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

#ifndef B2H_DEVICE_XIAOMI_LYWSD03MMC_HPP
#define B2H_DEVICE_XIAOMI_LYWSD03MMC_HPP

#include "ble/gatt/client.hpp"
#include "device/base.hpp"
#include "hass/device_types.hpp"
#include "mqtt/client.hpp"
#include "utils/json.hpp"
#include "utils/logger.hpp"

#include <array>
#include <functional>
#include <queue>
#include <thread>
#include <variant>

#include "boost/sml.hpp"

#include "host/ble_uuid.h"

namespace b2h::device::xiaomi
{
    namespace lywsd03mmc_impl
    {
        namespace events
        {
            struct connected {
            };

            struct disconnected {
            };

            struct notify {
                std::uint16_t attr_handle;
                std::vector<std::uint8_t> data;
            };

            struct srv_disced {
                tcb::span<ble::gatt::service> services;
            };

            struct chrs_disced {
                tcb::span<ble::gatt::characteristic> characteristics;
            };

            struct write_finished {
            };

            struct abort {
            };
        } // namespace events

        static constexpr std::string_view COMPONENT{ "xiaomi::lywsd03mmc" };

        static constexpr const char* TEMPERATURE_SENSOR_NAME{
            "LYWSD03MMC Temperature"
        };
        static constexpr const char* TEMPERATURE_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/lywsd03mmc_temperature/config"
        };
        static constexpr const char* TEMPERATURE_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/lywsd03mmc_temperature/state"
        };
        static constexpr const char* HUMIDITY_SENSOR_NAME{
            "LYWSD03MMC Humidity"
        };
        static constexpr const char* HUMIDITY_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/lywsd03mmc_humidity/config"
        };
        static constexpr const char* HUMIDITY_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/lywsd03mmc_humidity/state"
        };

        static constexpr ble_uuid128_t DATA_SRV{
            BLE_UUID_TYPE_128,
            {
                0xa6,
                0xa3,
                0x7d,
                0x99,
                0xf2,
                0x6f,
                0x1a,
                0x8a,
                0x0c,
                0x4b,
                0x0a,
                0x7a,
                0xb0,
                0xcc,
                0xe0,
                0xeb,
            },
        };

        static constexpr ble_uuid128_t DATA_CHR{
            BLE_UUID_TYPE_128,
            {
                0xa6,
                0xa3,
                0x7d,
                0x99,
                0xf2,
                0x6f,
                0x1a,
                0x8a,
                0x0c,
                0x4b,
                0x0a,
                0x7a,
                0xc1,
                0xcc,
                0xe0,
                0xeb,
            },
        };

        struct lywsd03mmc_state {
            using external_event_variant_t =
                std::variant<events::abort, events::srv_disced,
                    events::chrs_disced, events::write_finished>;

            ble::gatt::client& gatt_client;
            mqtt::client& mqtt_client;

            std::uint16_t data_attr_handle;

            float temperature;
            int humidity;

            std::function<void(external_event_variant_t)>
                process_external_event;
        };

        struct lywsd03mmc_fsm {
            auto operator()() noexcept
            {
                using namespace boost::sml;

                namespace sml = boost::sml;

                const auto find_by_uuid = [](const auto& cont,
                                              const auto& uuid) {
                    return std::find_if(cont.begin(),
                        cont.end(),
                        [&](const auto& attr) {
                            return ::ble_uuid_cmp(&attr.uuid.u, &uuid.u) == 0;
                        });
                };

                const auto write_handler = [](lywsd03mmc_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value())
                        {
                            log::warning(COMPONENT, "Write failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        log::debug(COMPONENT, "Write finished.");
                        state.process_external_event(events::write_finished{});
                    };
                };

                const auto on_start = [](lywsd03mmc_state& state) {
                    state.gatt_client.async_discover_service_by_uuid(
                        &DATA_SRV.u,
                        [&](auto&& result) {
                            if (!result.has_value() || result.value().empty())
                            {
                                log::error(COMPONENT,
                                    "Failed to discover DATA service by uuid.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state.process_external_event(
                                events::srv_disced{ result.value() });
                        });
                };

                const auto on_srv_disced = [](lywsd03mmc_state& state,
                                               events::srv_disced event) {
                    state.gatt_client.async_discover_characteristics(
                        event.services.front(),
                        [&](auto&& result) {
                            if (!result.has_value() || result.value().empty())
                            {
                                log::error(COMPONENT,
                                    "Failed to discover DATA characteristics.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state.process_external_event(
                                events::chrs_disced{ result.value() });
                        });
                };

                const auto on_chrs_disced = [=](lywsd03mmc_state& state,
                                                events::chrs_disced event,
                                                back::process<events::abort>
                                                    back_process) {
                    const auto data_it =
                        find_by_uuid(event.characteristics, DATA_CHR);
                    if (data_it == event.characteristics.end())
                    {
                        log::error(COMPONENT, "DATA characteristic not found.");
                        back_process(events::abort{});
                        return;
                    }

                    state.data_attr_handle = data_it->val_handle;

                    const std::array<std::uint8_t, 2> subscribe{ 1, 0 };

                    return state.gatt_client.async_write(state.data_attr_handle,
                        subscribe,
                        [=](auto&& result) {
                            if (!result.has_value())
                            {
                                log::error(COMPONENT,
                                    "Failed to subscribe to DATA characteristic.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state.process_external_event(
                                events::write_finished{});
                        });
                };

                const auto on_data_subscribe = [=](lywsd03mmc_state& state) {
                    hass::sensor_type temperature_sensor;
                    temperature_sensor.state_topic =
                        TEMPERATURE_SENSOR_STATE_TOPIC;
                    temperature_sensor.name         = TEMPERATURE_SENSOR_NAME;
                    temperature_sensor.device_class = "temperature";
                    temperature_sensor.unit_of_measurement = "°C";
                    temperature_sensor.qos                 = 1;

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(temperature_sensor)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_temp_sens = [=](lywsd03mmc_state& state) {
                    hass::sensor_type humidity_sensor;
                    humidity_sensor.state_topic  = HUMIDITY_SENSOR_STATE_TOPIC;
                    humidity_sensor.name         = HUMIDITY_SENSOR_NAME;
                    humidity_sensor.device_class = "humidity";
                    humidity_sensor.unit_of_measurement = "%";
                    humidity_sensor.qos                 = 1;

                    state.mqtt_client.async_publish(
                        HUMIDITY_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(humidity_sensor)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_data_handle = [](lywsd03mmc_state& state,
                                                const events::notify& event) {
                    return event.attr_handle == state.data_attr_handle;
                };

                const auto is_temp_upd = [](lywsd03mmc_state& state,
                                             const events::notify& event) {
                    const float temperature =
                        static_cast<float>(
                            event.data[0] | (event.data[1] << 8)) *
                        0.01f;

                    if (temperature == state.temperature)
                    {
                        return false;
                    }

                    state.temperature = temperature;
                    return true;
                };

                const auto upd_temp = [=](lywsd03mmc_state& state) {
                    using buff_t = std::array<char, 6>;

                    buff_t buff;

                    const auto [iter, size] = fmt::format_to_n(buff.begin(),
                        buff.size(),
                        "{:.1f}",
                        state.temperature);

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SENSOR_STATE_TOPIC,
                        std::string_view{
                            buff.data(),
                            size,
                        },
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_hum_upd = [](lywsd03mmc_state& state,
                                            const events::notify& event) {
                    if (state.humidity == event.data[2])
                    {
                        return false;
                    }

                    state.humidity = event.data[2];
                    return true;
                };

                const auto upd_hum = [=](lywsd03mmc_state& state) {
                    using buff_t = std::array<char, 4>;

                    buff_t buff;

                    const auto [iter, size] = fmt::format_to_n(buff.begin(),
                        buff.size(),
                        "{}",
                        state.humidity);

                    state.mqtt_client.async_publish(HUMIDITY_SENSOR_STATE_TOPIC,
                        std::string_view{
                            buff.data(),
                            size,
                        },
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_abort_conn = [](lywsd03mmc_state& state) {
                    log::warning(COMPONENT, "Terminating BLE connection.");
                    state.gatt_client.terminate();
                };

                // clang-format off
                return make_transition_table(
                    *"idle"_s + sml::event<events::connected> / on_start = "disc_data_srv"_s,

                    "disc_data_srv"_s + sml::event<events::srv_disced> / on_srv_disced = "disc_data_chrs"_s,
                    "disc_data_srv"_s + sml::event<events::abort>                      = "terminate"_s,

                    "disc_data_chrs"_s + sml::event<events::chrs_disced> / on_chrs_disced = "data_subscribe"_s,
                    "disc_data_chrs"_s + sml::event<events::abort>                        = "terminate"_s,

                    "data_subscribe"_s + sml::event<events::write_finished> / on_data_subscribe = "conf_temp_sens"_s,
                    "data_subscribe"_s + sml::event<events::abort>                              = "terminate"_s,

                    "conf_temp_sens"_s + sml::event<events::write_finished> / on_conf_temp_sens = "conf_humi_sens"_s,
                    "conf_temp_sens"_s + sml::event<events::abort>                              = "terminate"_s,

                    "conf_humi_sens"_s + sml::event<events::write_finished> = "operate"_s,
                    "conf_humi_sens"_s + sml::event<events::abort>          = "terminate"_s,
            
                    "operate"_s + sml::event<events::notify> [is_data_handle && is_hum_upd]  / upd_hum  = "param_write"_s,
                    "operate"_s + sml::event<events::notify> [is_data_handle && is_temp_upd] / upd_temp = "param_write"_s,

                    "operate"_s + sml::event<events::abort>        = "terminate"_s,
                    "operate"_s + sml::event<events::disconnected> = X,

                    "param_write"_s + sml::event<events::write_finished> = "operate"_s,
                    "param_write"_s + sml::event<events::abort>          = "operate"_s,
                    "param_write"_s + sml::event<events::disconnected>   = X,

                    "terminate"_s + on_entry<_> / on_abort_conn
                );
                // clang-format on
            }
        };
    } // namespace lywsd03mmc_impl

    class lywsd03mmc final : public base
    {
    public:
        lywsd03mmc(std::unique_ptr<mqtt::client>&& mqtt_client,
            std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept;

        lywsd03mmc()                  = delete;
        lywsd03mmc(const lywsd03mmc&) = delete;
        lywsd03mmc(lywsd03mmc&&)      = default;
        ~lywsd03mmc()                 = default;

        lywsd03mmc& operator=(const lywsd03mmc&) = delete;
        lywsd03mmc& operator=(lywsd03mmc&&) = default;

        void on_connected() noexcept override;
        void on_notify(std::uint16_t attribute_handle,
            std::vector<std::uint8_t>&& data) noexcept override;

    private:
        using fsm_t = boost::sml::sm<lywsd03mmc_impl::lywsd03mmc_fsm,
            boost::sml::thread_safe<std::recursive_mutex>,
            boost::sml::process_queue<std::queue>>;

        lywsd03mmc_impl::lywsd03mmc_state m_state;
        fsm_t m_fsm;

        auto make_process_external_event()
        {
            using external_event_variant_t =
                lywsd03mmc_impl::lywsd03mmc_state::external_event_variant_t;
            return [this](const external_event_variant_t& event_variant) {
                std::visit(
                    [this](const auto& arg) { m_fsm.process_event(arg); },
                    event_variant);
            };
        }
    };
} // namespace b2h::device::xiaomi

#endif
