// Copyright 2022 Borys Chyliński

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ble/ble.hpp"
#include "ble/gap/central.hpp"
#include "ble/gatt/client.hpp"
#include "device/base.hpp"
#include "device/builder.hpp"
#include "event/event.hpp"
#include "mqtt/client.hpp"
#include "utils/esp_exception.hpp"
#include "utils/json.hpp"
#include "utils/logger.hpp"
#include "wifi/station.hpp"

#include "esp_event.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"

#include <algorithm>
#include <atomic>
#include <future>
#include <list>
#include <string>
#include <string_view>
#include <thread>

namespace b2h
{
    class application
    {
    public:
        application() noexcept :
            m_config{ load_config() },
            m_context{},
            m_station{ m_context },
            m_exit{ false },
            m_connection_task{}
        {
        }

        ~application()
        {
            if (m_exit = true; m_connection_task.joinable())
            {
                m_connection_task.join();
            }
        }

        void run()
        {
            async_init();
            m_connection_task = std::thread([this]() { connection_task(); });
            m_context.run();
        }

    private:
        struct device_config {
            std::string mac;
            std::string name;
        };

        struct app_config {
            std::string wifi_ssid;
            std::string wifi_password;

            std::string mqtt_broker_uri;
            std::string mqtt_user;
            std::string mqtt_password;

            std::vector<device_config> devices;
        };

        using device_container =
            std::vector<std::pair<std::reference_wrapper<const device_config>,
                std::weak_ptr<device::interface>>>;

        static constexpr std::string_view COMPONENT{ "application" };

        const app_config m_config;
        event::context m_context;
        wifi::station m_station;

        std::atomic_bool m_exit;
        std::thread m_connection_task;

        static app_config load_config()
        {
            constexpr const char* CONFIG_FILE_PATH{ "/spiffs/config.json" };

            constexpr auto get_string_value = [=](const auto& doc,
                                                  const char* key) {
                const auto iter = doc.FindMember(key);
                if (iter == doc.MemberEnd())
                {
                    throw std::runtime_error{ fmt::format(
                        "Field \"{}\" not found in {}.",
                        key,
                        CONFIG_FILE_PATH) };
                }

                if (!iter->value.IsString())
                {
                    throw std::runtime_error{
                        fmt::format("Field \"{}\" must be a string.", key)
                    };
                }

                return std::string{
                    iter->value.GetString(),
                    iter->value.GetStringLength(),
                };
            };

            constexpr auto get_optional_string_value = [=](const auto& doc,
                                                           const char* key) {
                const auto iter = doc.FindMember(key);
                if (iter == doc.MemberEnd())
                {
                    return std::string{};
                }

                if (!iter->value.IsString())
                {
                    throw std::runtime_error{ fmt::format(
                        "Optional field \"{}\" must be a string.",
                        key) };
                }

                return std::string{ iter->value.GetString() };
            };

            constexpr auto get_array_value = [=](const auto& doc,
                                                 const char* key) {
                const auto iter = doc.FindMember(key);
                if (iter == doc.MemberEnd())
                {
                    throw std::runtime_error{ fmt::format(
                        "Field \"{}\" not found in {}.",
                        key,
                        CONFIG_FILE_PATH) };
                }

                if (!iter->value.IsArray())
                {
                    throw std::runtime_error{
                        fmt::format("Field \"{}\" must be an array.", key)
                    };
                }

                return iter->value.GetArray();
            };

            app_config config;
            const auto json_config = utils::json::parse_file(CONFIG_FILE_PATH);

            if (!json_config.IsObject())
            {
                throw std::runtime_error("Invalid \"config.json\".");
            }

            config.wifi_ssid = get_string_value(json_config, "wifi_ssid");
            config.wifi_password =
                get_string_value(json_config, "wifi_password");
            config.mqtt_broker_uri =
                get_string_value(json_config, "mqtt_broker_uri");

            config.mqtt_user =
                get_optional_string_value(json_config, "mqtt_user");
            config.mqtt_password =
                get_optional_string_value(json_config, "mqtt_password");

            for (const auto& elem : get_array_value(json_config, "devices"))
            {
                config.devices.push_back({
                    get_string_value(elem, "mac"),
                    get_string_value(elem, "name"),
                });
            }

            return config;
        }

        void async_init() noexcept
        {
            m_station.config(wifi::station_config{
                m_config.wifi_ssid,
                m_config.wifi_password,
            });

            async_wifi_start();
        }

        void async_wifi_start() noexcept
        {
            m_station.async_start([this](auto&& result) {
                if (!result.has_value())
                {
                    throw utils::esp_exception{ "Failed to start WiFi station.",
                        result.error() };
                }

                async_wifi_connect();
            });
        }

        void async_wifi_connect() noexcept
        {
            m_station.async_connect([this](auto&& result) {
                if (!result.has_value())
                {
                    log::error(COMPONENT,
                        "WiFi connection failed. Error code: {} [{}].",
                        result.error(),
                        esp_err_to_name(result.error()));
                    async_wifi_connect();
                    return;
                }

                log::info(COMPONENT, "Successfully connected to WiFi.");
                m_station.on_disconnect([this](auto&&) {
                    log::warning(COMPONENT, "WiFi disconnected.");
                    async_wifi_connect();
                });
            });
        }

        template<typename ContainerT>
        static void async_ble_notify_rx(
            ble::gap::central& gap_central, ContainerT& cont) noexcept
        {
            gap_central.async_notify_rx([&](auto&& data) mutable {
                if (!data.has_value())
                {
                    log::error(COMPONENT,
                        "Error receiving the notification. Error code: {}",
                        data.error());
                    return;
                }

                auto device_iter = std::find_if(cont.begin(),
                    cont.end(),
                    [connection_handle{ data.value().connection_handle }](
                        const auto& device_ptr) {
                        return device_ptr->connection_handle() ==
                               connection_handle;
                    });

                if (device_iter != cont.end())
                {
                    (*device_iter)
                        ->on_notify(data.value().attribute_handle,
                            std::move(data.value().data));
                }

                async_ble_notify_rx(gap_central, cont);
            });
        }

        template<typename ContainerT>
        static void async_ble_on_disconnect(
            ble::gap::central& gap_central, ContainerT& cont) noexcept
        {
            gap_central.on_disconnect([&](auto&& data) mutable {
                if (!data.has_value())
                {
                    log::error(COMPONENT,
                        "Error receiving disconnect event. Error code: {}",
                        data.error());
                    return;
                }

                auto device_iter = std::find_if(cont.begin(),
                    cont.end(),
                    [connection_handle{ data.value() }](
                        const auto& device_ptr) {
                        return device_ptr->connection_handle() ==
                               connection_handle;
                    });

                if (device_iter != cont.end())
                {
                    (*device_iter)->on_disconnected();
                    cont.erase(device_iter); // Drop the ownership over the
                                             // device object.
                }

                async_ble_on_disconnect(gap_central, cont);
            });
        }

        void connection_task() noexcept
        {
            using namespace std::literals;

            const auto ble_ctrl_sync_await = [this](ble::context& ctx) {
                using arg_t = events::ble::init::expected_type;
                std::packaged_task<arg_t(arg_t &&)> sync_task{
                    [](arg_t&& arg) { return arg; },
                };

                auto sync_future = sync_task.get_future();
                ctx.async_init(std::ref(sync_task));
                return sync_future.get();
            };

            device_container devices;
            std::list<std::shared_ptr<device::interface>> device_refs;

            devices.reserve(m_config.devices.size());
            for (const auto& cfg : m_config.devices)
            {
                devices.emplace_back(std::cref(cfg),
                    std::weak_ptr<device::interface>{});
            }

            ble::context ble_context{ m_context };

            log::info(COMPONENT, "Waiting for BLE controller synchronization.");
            if (!ble_ctrl_sync_await(ble_context).has_value())
            {
                log::critical(COMPONENT,
                    "BLE controller synchronization failed.");
                return;
            }

            ble::gap::central gap_central{ m_context };

            async_ble_notify_rx(gap_central, device_refs);
            async_ble_on_disconnect(gap_central, device_refs);

            while (!m_exit)
            {
                for (auto& [device_config, device_ptr] : devices)
                {
                    if (!device_ptr.expired())
                    {
                        log::debug(COMPONENT,
                            "Device \"{}\" [MAC: \"{}\"] remains connected.",
                            device_config.get().name,
                            device_config.get().mac);
                        continue;
                    }

                    log::info(COMPONENT,
                        "Attempting to connect to device: \"{}\" [MAC: \"{}\"].",
                        device_config.get().name,
                        device_config.get().mac);

                    try
                    {
                        device::builder builder{};
                        auto device_shared =
                            builder.context(m_context)
                                .mqtt_client({
                                    m_config.mqtt_broker_uri,
                                    m_config.mqtt_user,
                                    m_config.mqtt_password,
                                })
                                .gatt_client(gap_central,
                                    utils::make_mac(device_config.get().mac)
                                        .value())
                                .build(device_config.get().name);

                        device_ptr =
                            device_refs.emplace_back(std::move(device_shared));
                    }
                    catch (const std::exception& err)
                    {
                        log::error(COMPONENT, err.what());
                    }
                }

                log::debug(COMPONENT,
                    "Connection task sleeping for 30 seconds.");
                std::this_thread::sleep_for(30s);
            }
        }
    };
} // namespace b2h

extern "C" void app_main(void) noexcept
{
    using namespace b2h;
    using namespace std::literals;

    constexpr std::string_view COMPONENT{ "app_main" };
    constexpr esp_vfs_spiffs_conf_t fs_config{
        "/spiffs", // base path
        "storage", // partition label
        1,         // max_files
        true       // format if mount failed
    };

    esp_err_t result = ESP_OK;

    if (result = nvs_flash_init(); result != ESP_OK)
    {
        log::critical(COMPONENT,
            "Failed to initialize NVS flash. Error code: {} [{}]",
            result,
            esp_err_to_name(result));
        return;
    }

    if (result = esp_vfs_spiffs_register(&fs_config); result != ESP_OK)
    {
        log::critical(COMPONENT,
            "Failed to initialize SPIFFS filesystem. Error code: {} [{}]",
            result,
            esp_err_to_name(result));
        goto err_nvs_deinit;
    }

    if (result = esp_event_loop_create_default(); result != ESP_OK)
    {
        log::critical(COMPONENT,
            "Default event loop creation failed. Error code: {} [{}]",
            result,
            esp_err_to_name(result));
        goto spiffs_deinit;
    }

    try
    {
        application app;

        log::info(COMPONENT, "Starting event context.");
        app.run();
    }
    catch (const std::exception& err)
    {
        log::error(COMPONENT, "Exception caught: {}", err.what());
    }
    catch (...)
    {
        log::critical(COMPONENT, "Unknown exception caught.");
    }

    log::warning(COMPONENT, "Application exited, cleaning up resources.");

    esp_event_loop_delete_default();
spiffs_deinit:
    esp_vfs_spiffs_unregister(fs_config.partition_label);
err_nvs_deinit:
    nvs_flash_deinit();
}
