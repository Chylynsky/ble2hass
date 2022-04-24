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
#include "fmt/format.h"

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

        static constexpr std::string_view DEVICE_NAME{
            "Mi Temperature & Humidity Monitor 2"
        };

        static constexpr std::string_view DEVICE_MODEL{ "LYWSD03MMC" };

        static constexpr std::string_view DEVICE_MANUFACTURER{ "Xiaomi" };

        static constexpr std::string_view TEMPERATURE_SENSOR_NAME{
            "LYWSD03MMC Temperature"
        };

        static constexpr std::string_view TEMPERATURE_SENSOR_UNIQUE_ID_TMPL{
            "lywsd03mmc_{}_temperature"
        };

        static constexpr std::string_view HUMIDITY_SENSOR_NAME{
            "LYWSD03MMC Humidity"
        };

        static constexpr std::string_view HUMIDITY_SENSOR_UNIQUE_ID_TMPL{
            "lywsd03mmc_{}_humidity"
        };

        static constexpr std::string_view BATTERY_SENSOR_NAME{
            "LYWSD03MMC Battery"
        };

        static constexpr std::string_view BATTERY_SENSOR_UNIQUE_ID_TMPL{
            "lywsd03mmc_{}_battery"
        };

        static constexpr std::string_view SENSOR_CONFIG_TOPIC_TMPL{
            "homeassistant/sensor/{}/config"
        };
        static constexpr std::string_view SENSOR_STATE_TOPIC_TMPL{
            "homeassistant/sensor/{}/state"
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

        using topic_buffer_t     = std::array<char, 64>;
        using unique_id_buffer_t = std::array<char, 35>;
        using id_buffer_t        = std::array<char, 12>;
        using meas_buffer_t      = std::array<char, 5>;

        struct lywsd03mmc_state {
            struct configure {
                using mac_buffer_t = std::array<char, utils::mac::MAC_STR_SIZE>;
                using connection_tuple_t =
                    std::pair<std::string_view, std::string_view>;
                using connections_list_t = std::array<connection_tuple_t, 1>;

                mac_buffer_t mac_buf;
                connections_list_t conn;
            };

            struct operate {
                std::uint16_t temperature;
                std::uint16_t voltage;
                std::uint8_t humidity;
            };

            using external_event_variant_t =
                std::variant<events::abort, events::srv_disced,
                    events::chrs_disced, events::write_finished>;

            using state_variant_t =
                std::variant<std::monostate, configure, operate>;

            ble::gatt::client& gatt_client;
            mqtt::client& mqtt_client;

            topic_buffer_t temp_sens_topic;
            topic_buffer_t hum_sens_topic;
            topic_buffer_t batt_sens_topic;

            state_variant_t state_var;

            std::uint16_t data_attr_handle;

            std::function<void(external_event_variant_t)>
                process_external_event;
        };

        struct lywsd03mmc_fsm {
            auto operator()() noexcept
            {
                using namespace boost::sml;

                namespace sml = boost::sml; // ambiguity between
                                            // boost::sml::event and b2h::event

                const auto find_by_uuid = [](const auto& cont,
                                              const auto& uuid) {
                    return std::find_if(cont.begin(),
                        cont.end(),
                        [&](const auto& attr) {
                            return ::ble_uuid_cmp(&attr.uuid.u, &uuid.u) == 0;
                        });
                };

                const auto mac_to_id = [](const utils::mac& mac,
                                           id_buffer_t& buf) {
                    const std::uint8_t* mac_bytes = mac.as_bytes();
                    const auto [out, size] = fmt::format_to_n(buf.begin(),
                        buf.size(),
                        "{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
                        mac_bytes[5],
                        mac_bytes[4],
                        mac_bytes[3],
                        mac_bytes[2],
                        mac_bytes[1],
                        mac_bytes[0]);

                    return std::string_view{ buf.data(), size };
                };

                const auto make_unique_id = [=](const std::string_view tmpl,
                                                const utils::mac& mac,
                                                unique_id_buffer_t& buf) {
                    id_buffer_t id_buf{};
                    const auto [out, size] = fmt::format_to_n(buf.begin(),
                        buf.size(),
                        tmpl,
                        mac_to_id(mac, id_buf));

                    return std::string_view{ buf.data(), size };
                };

                const auto make_topic =
                    [=](const std::string_view topic_tmpl,
                        const std::string_view unique_id_tmpl,
                        const utils::mac& mac,
                        topic_buffer_t& buf) {
                        unique_id_buffer_t unique_id_buf{};

                        const auto [out, size] = fmt::format_to_n(buf.begin(),
                            buf.size() - 1,
                            topic_tmpl,
                            make_unique_id(unique_id_tmpl, mac, unique_id_buf));

                        assert(size != buf.size());
                        *out = '\0';
                        return buf.data();
                    };

                const auto make_state_topic =
                    [=](const std::string_view unique_id_tmpl,
                        const utils::mac& mac,
                        topic_buffer_t& buf) {
                        return make_topic(SENSOR_STATE_TOPIC_TMPL,
                            unique_id_tmpl,
                            mac,
                            buf);
                    };

                const auto make_config_topic =
                    [=](const std::string_view unique_id_tmpl,
                        const utils::mac& mac,
                        topic_buffer_t& buf) {
                        return make_topic(SENSOR_CONFIG_TOPIC_TMPL,
                            unique_id_tmpl,
                            mac,
                            buf);
                    };

                const auto make_hass_device =
                    [](lywsd03mmc_state::configure& state) {
                        hass::device_type device;

                        device.name         = DEVICE_NAME;
                        device.model        = DEVICE_MODEL;
                        device.manufacturer = DEVICE_MANUFACTURER;
                        device.connections  = tcb::make_span(state.conn);

                        return device;
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

                auto on_start = [=](lywsd03mmc_state& state) mutable {
                    using namespace std::literals;

                    const utils::mac& mac = state.gatt_client.mac();

                    auto& state_var =
                        state.state_var
                            .template emplace<lywsd03mmc_state::configure>();

                    state_var.conn[0].first = "mac"sv;
                    state_var.conn[0].second =
                        mac.to_charbuf(state_var.mac_buf.begin(),
                               state_var.mac_buf.end())
                            .value();

                    make_state_topic(TEMPERATURE_SENSOR_UNIQUE_ID_TMPL,
                        mac,
                        state.temp_sens_topic);

                    make_state_topic(HUMIDITY_SENSOR_UNIQUE_ID_TMPL,
                        mac,
                        state.hum_sens_topic);

                    make_state_topic(BATTERY_SENSOR_UNIQUE_ID_TMPL,
                        mac,
                        state.batt_sens_topic);

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

                auto on_data_subscribe = [=](lywsd03mmc_state& state) mutable {
                    using namespace std::literals;

                    const utils::mac& mac = state.gatt_client.mac();

                    topic_buffer_t topic_buf;
                    unique_id_buffer_t unique_id_buf{};
                    hass::sensor_type sens;

                    auto& state_var =
                        std::get<lywsd03mmc_state::configure>(state.state_var);

                    sens.device              = make_hass_device(state_var);
                    sens.state_topic         = state.temp_sens_topic.data();
                    sens.name                = TEMPERATURE_SENSOR_NAME;
                    sens.device_class        = "temperature"sv;
                    sens.unit_of_measurement = "°C"sv;
                    sens.qos                 = 0;
                    sens.unique_id =
                        make_unique_id(TEMPERATURE_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            unique_id_buf);
                    sens.value_template =
                        "{{ ((value_json | float(0)) * 0.01) | round(2) }}"sv;

                    state.mqtt_client.async_publish(
                        make_config_topic(TEMPERATURE_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            topic_buf),
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                auto on_conf_temp_sens = [=](lywsd03mmc_state& state) mutable {
                    using namespace std::literals;

                    const utils::mac& mac = state.gatt_client.mac();

                    topic_buffer_t topic_buf;
                    unique_id_buffer_t unique_id_buf{};
                    hass::sensor_type sens;

                    auto& state_var =
                        std::get<lywsd03mmc_state::configure>(state.state_var);

                    sens.device              = make_hass_device(state_var);
                    sens.state_topic         = state.hum_sens_topic.data();
                    sens.name                = HUMIDITY_SENSOR_NAME;
                    sens.device_class        = "humidity"sv;
                    sens.unit_of_measurement = "%"sv;
                    sens.qos                 = 0;
                    sens.unique_id =
                        make_unique_id(HUMIDITY_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            unique_id_buf);

                    state.mqtt_client.async_publish(
                        make_config_topic(HUMIDITY_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            topic_buf),
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                auto on_conf_humi_sens = [=](lywsd03mmc_state& state) mutable {
                    using namespace std::literals;

                    const utils::mac& mac = state.gatt_client.mac();

                    topic_buffer_t topic_buf;
                    unique_id_buffer_t unique_id_buf{};
                    hass::sensor_type sens;

                    auto& state_var =
                        std::get<lywsd03mmc_state::configure>(state.state_var);

                    sens.device              = make_hass_device(state_var);
                    sens.state_topic         = state.batt_sens_topic.data();
                    sens.name                = BATTERY_SENSOR_NAME;
                    sens.device_class        = "battery"sv;
                    sens.unit_of_measurement = "%"sv;
                    sens.qos                 = 0;
                    sens.unique_id =
                        make_unique_id(BATTERY_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            unique_id_buf);
                    sens.value_template =
                        "{% if value_json >= 3000 %}"
                        "{{ 100 }}"
                        "{% elif value_json <= 2100 %}"
                        "{{ 0 }}"
                        "{% else %}"
                        "{{ (100.0 * (((value_json | float(0)) - 2100.0) / 900.0)) | round(0) }}"
                        "{% endif %}"sv;

                    state.mqtt_client.async_publish(
                        make_config_topic(BATTERY_SENSOR_UNIQUE_ID_TMPL,
                            mac,
                            topic_buf),
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto set_operate = [](lywsd03mmc_state& state) {
                    state.state_var
                        .template emplace<lywsd03mmc_state::operate>();
                };

                const auto is_data_handle = [](lywsd03mmc_state& state,
                                                const events::notify& event) {
                    return event.attr_handle == state.data_attr_handle;
                };

                const auto is_temp_upd = [](lywsd03mmc_state& state,
                                             const events::notify& event) {
                    const std::uint16_t temperature =
                        (static_cast<std::uint16_t>(event.data[1]) << 8) |
                        event.data[0];

                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);

                    if (temperature == state_var.temperature)
                    {
                        return false;
                    }

                    state_var.temperature = temperature;
                    return true;
                };

                const auto upd_temp = [=](lywsd03mmc_state& state) {
                    using namespace std::literals;

                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);
                    meas_buffer_t buff;

                    const auto [iter, size] = fmt::format_to_n(buff.begin(),
                        buff.size(),
                        "{}",
                        state_var.temperature);

                    state.mqtt_client.async_publish(
                        state.temp_sens_topic.data(),
                        std::string_view{
                            buff.data(),
                            size,
                        },
                        0,
                        true,
                        write_handler(state));
                };

                const auto is_hum_upd = [](lywsd03mmc_state& state,
                                            const events::notify& event) {
                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);

                    if (state_var.humidity == event.data[2])
                    {
                        return false;
                    }

                    state_var.humidity = event.data[2];
                    return true;
                };

                const auto upd_hum = [=](lywsd03mmc_state& state) {
                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);
                    meas_buffer_t buff;

                    const auto [iter, size] = fmt::format_to_n(buff.begin(),
                        buff.size(),
                        "{}",
                        state_var.humidity);

                    state.mqtt_client.async_publish(state.hum_sens_topic.data(),
                        std::string_view{
                            buff.data(),
                            size,
                        },
                        0,
                        true,
                        write_handler(state));
                };

                const auto is_batt_upd = [](lywsd03mmc_state& state,
                                             const events::notify& event) {
                    const std::uint16_t voltage =
                        (static_cast<std::uint16_t>(event.data[4]) << 8) |
                        event.data[3];

                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);

                    if (state_var.voltage == voltage)
                    {
                        return false;
                    }

                    state_var.voltage = voltage;
                    return true;
                };

                const auto upd_batt = [=](lywsd03mmc_state& state) {
                    auto& state_var =
                        std::get<lywsd03mmc_state::operate>(state.state_var);
                    meas_buffer_t buff;

                    const auto [iter, size] = fmt::format_to_n(buff.begin(),
                        buff.size(),
                        "{}",
                        state_var.voltage);

                    state.mqtt_client.async_publish(
                        state.batt_sens_topic.data(),
                        std::string_view{
                            buff.data(),
                            size,
                        },
                        0,
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

                    "conf_humi_sens"_s + sml::event<events::write_finished> / on_conf_humi_sens = "conf_batt_sens"_s,
                    "conf_humi_sens"_s + sml::event<events::abort>                              = "terminate"_s,

                    "conf_batt_sens"_s + sml::event<events::write_finished> / set_operate = "operate"_s,
                    "conf_batt_sens"_s + sml::event<events::abort>                        = "terminate"_s,
            
                    "operate"_s + sml::event<events::notify> [is_data_handle && is_batt_upd] / upd_batt = "param_write"_s,
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
