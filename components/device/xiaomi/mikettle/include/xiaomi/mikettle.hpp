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

#ifndef B2H_DEVICE_XIAOMI_MIKETTLE_HPP
#define B2H_DEVICE_XIAOMI_MIKETTLE_HPP

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <queue>
#include <string_view>
#include <thread>
#include <vector>

#include "boost/sml.hpp"
#include "fmt/format.h"

#include "ble/gatt/client.hpp"
#include "device/base.hpp"
#include "hass/device_types.hpp"
#include "mqtt/client.hpp"
#include "utils/const_map.hpp"
#include "utils/json.hpp"

#include "host/ble_uuid.h"

namespace b2h::device::xiaomi
{
    namespace mikettle_impl
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

            struct descs_disced {
                tcb::span<ble::gatt::descriptor> descriptors;
            };

            struct write_finished {
            };

            struct read_finished {
                tcb::span<std::uint8_t> data;
            };

            struct mqtt_data {
                std::string_view topic;
                std::string_view data;
            };

            struct sub_finished {
            };

            struct abort {
            };
        }; // namespace events

        inline constexpr std::string_view COMPONENT{ "xiaomi::mikettle" };

        inline constexpr std::uint8_t KEY_LENGTH{ 4 };
        inline constexpr std::uint8_t TOKEN_LENGTH{ 12 };
        inline constexpr std::uint16_t PERM_LENGTH{ 256 };

        inline constexpr std::uint16_t PRODUCT_ID{ 275 };

        inline constexpr std::array<std::uint8_t, KEY_LENGTH> key1{
            0x90,
            0xCA,
            0x85,
            0xDE,
        };

        inline constexpr std::array<std::uint8_t, KEY_LENGTH> key2{
            0x92,
            0xAB,
            0x54,
            0xFA,
        };

        inline constexpr std::array<std::uint8_t, TOKEN_LENGTH> token{
            0x91,
            0xf5,
            0x80,
            0x93,
            0x24,
            0x49,
            0xb4,
            0x0d,
            0x6b,
            0x06,
            0xd2,
            0x8a,
        };

        inline constexpr std::array<std::uint8_t, 2> subscribe{
            0x01,
            0x00,
        };

        inline constexpr std::array<std::uint8_t, 2> unsubscribe{
            0x00,
            0x00,
        };

        /* MiKettle services */

        inline constexpr ble_uuid16_t GATT_UUID_KETTLE_SRV{
            BLE_UUID_TYPE_16,
            0xfe95,
        };

        inline constexpr ble_uuid128_t GATT_UUID_KETTLE_DATA_SRV{
            BLE_UUID_TYPE_128,
            {
                0x56,
                0x61,
                0x23,
                0x37,
                0x28,
                0x26,
                0x00,
                0x80,
                0x00,
                0x10,
                0x00,
                0x00,
                0x36,
                0x47,
                0x34,
                0x01,
            },
        };

        /* MiKettle characteristics */

        inline constexpr ble_uuid16_t GATT_UUID_AUTH_INIT{
            BLE_UUID_TYPE_16,
            0x0010,
        };
        inline constexpr ble_uuid16_t GATT_UUID_AUTH{
            BLE_UUID_TYPE_16,
            0x0001,
        };
        inline constexpr ble_uuid16_t GATT_UUID_VERSION{
            BLE_UUID_TYPE_16,
            0x0004,
        };
        inline constexpr ble_uuid16_t GATT_UUID_SETUP{
            BLE_UUID_TYPE_16,
            0xaa01,
        };
        inline constexpr ble_uuid16_t GATT_UUID_STATUS{
            BLE_UUID_TYPE_16,
            0xaa02,
        };
        inline constexpr ble_uuid16_t GATT_UUID_TIME{
            BLE_UUID_TYPE_16,
            0xaa04,
        };
        inline constexpr ble_uuid16_t GATT_UUID_BOIL_MODE{
            BLE_UUID_TYPE_16,
            0xaa05,
        };
        inline constexpr ble_uuid16_t GATT_UUID_MCU_VERSION{
            BLE_UUID_TYPE_16,
            0x2a28,
        };
        inline constexpr ble_uuid16_t GATT_UUID_CCCD{
            BLE_UUID_TYPE_16,
            0x2902,
        };

        inline constexpr const char* TEMPERATURE_SENSOR_NAME{
            "MiKettle Temperature"
        };

        inline constexpr const char* TEMPERATURE_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/mikettle_temperature/config"
        };

        inline constexpr const char* TEMPERATURE_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/mikettle_temperature/state"
        };

        inline constexpr const char* ACTION_SENSOR_NAME{ "MiKettle Action" };

        inline constexpr const char* ACTION_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/mikettle_action/config"
        };

        inline constexpr const char* ACTION_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/mikettle_action/state"
        };

        inline constexpr const char* MODE_SENSOR_NAME{ "MiKettle Mode" };

        inline constexpr const char* MODE_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/mikettle_mode/config"
        };

        inline constexpr const char* MODE_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/mikettle_mode/state"
        };

        inline constexpr const char* KEEP_WARM_TIME_SENSOR_NAME{
            "MiKettle Keep Warm Time"
        };

        inline constexpr const char* KEEP_WARM_TIME_SENSOR_CONFIG_TOPIC{
            "homeassistant/sensor/mikettle_keep_warm_time/config"
        };

        inline constexpr const char* KEEP_WARM_TIME_SENSOR_STATE_TOPIC{
            "homeassistant/sensor/mikettle_keep_warm_time/state"
        };

        inline constexpr const char* TEMPERATURE_SET_NUMBER_NAME{
            "MiKettle Temperature Set"
        };

        inline constexpr const char* TEMPERATURE_SET_NUMBER_CONFIG_TOPIC{
            "homeassistant/number/mikettle_temperature_set/config"
        };

        inline constexpr const char* TEMPERATURE_SET_NUMBER_STATE_TOPIC{
            "homeassistant/number/mikettle_temperature_set/state"
        };

        inline constexpr const char* TEMPERATURE_SET_NUMBER_CMD_TOPIC{
            "homeassistant/number/mikettle_temperature_set/cmd"
        };

        inline constexpr const char* KEEP_WARM_TIME_LIMIT_NUMBER_NAME{
            "MiKettle Keep Warm Time Limit"
        };

        inline constexpr const char* KEEP_WARM_TIME_LIMIT_NUMBER_CONFIG_TOPIC{
            "homeassistant/number/mikettle_keep_warm_time_limit/config"
        };

        inline constexpr const char* KEEP_WARM_TIME_LIMIT_NUMBER_STATE_TOPIC{
            "homeassistant/number/mikettle_keep_warm_time_limit/state"
        };

        inline constexpr const char* KEEP_WARM_TIME_LIMIT_NUMBER_CMD_TOPIC{
            "homeassistant/number/mikettle_keep_warm_time_limit/cmd"
        };

        inline constexpr const char* KEEP_WARM_TYPE_SELECT_NAME{
            "MiKettle Keep Warm Type"
        };

        inline constexpr const char* KEEP_WARM_TYPE_SELECT_CONFIG_TOPIC{
            "homeassistant/select/mikettle_keep_warm_type/config"
        };

        inline constexpr const char* KEEP_WARM_TYPE_SELECT_STATE_TOPIC{
            "homeassistant/select/mikettle_keep_warm_type/state"
        };

        inline constexpr const char* KEEP_WARM_TYPE_SELECT_CMD_TOPIC{
            "homeassistant/select/mikettle_keep_warm_type/cmd"
        };

        inline constexpr const char* TURN_OFF_AFTER_BOIL_SWITCH_NAME{
            "MiKettle Turn Off After Boil"
        };

        inline constexpr const char* TURN_OFF_AFTER_BOIL_SWITCH_CONFIG_TOPIC{
            "homeassistant/switch/mikettle_turn_off_after_boil/config"
        };

        inline constexpr const char* TURN_OFF_AFTER_BOIL_SWITCH_STATE_TOPIC{
            "homeassistant/switch/mikettle_turn_off_after_boil/state"
        };

        inline constexpr const char* TURN_OFF_AFTER_BOIL_SWITCH_CMD_TOPIC{
            "homeassistant/switch/mikettle_turn_off_after_boil/cmd"
        };

        inline constexpr std::string_view ACTION_IDLE{ "idle" };
        inline constexpr std::string_view ACTION_HEATING{ "heating" };
        inline constexpr std::string_view ACTION_COOLING{ "cooling" };
        inline constexpr std::string_view ACTION_KEEPING_WARM{ "keeping warm" };

        inline constexpr std::string_view MODE_NONE{ "none" };
        inline constexpr std::string_view MODE_BOIL{ "boil" };
        inline constexpr std::string_view MODE_WARM{ "warm" };

        inline constexpr std::string_view KEEP_WARM_TYPE_BOIL_AND_COOL{
            "boil and cool down to set temperature"
        };
        inline constexpr std::string_view KEEP_WARM_TYPE_HEAT_UP{
            "heat up to set temperature"
        };

        template<typename InputT, typename KeyT>
        std::vector<std::uint8_t> cipher(const KeyT& key, const InputT& input)
        {
            std::vector<std::uint8_t> result;
            std::array<std::uint8_t, 256> perm;

            result.reserve(input.size());

            std::size_t index1 = 0;
            std::size_t index2 = 0;
            std::size_t index3 = 0;

            std::iota(perm.begin(), perm.end(), 0);

            size_t j = 0;
            for (size_t i = 0; i < perm.size(); i++)
            {
                j += perm[i] + key[i % key.size()];
                j &= 0xff;
                std::swap(perm[i], perm[j]);
            }

            for (size_t i = 0; i < input.size(); i++)
            {
                ++index1;
                index1 &= 0xff;
                index2 += perm[index1];
                index2 &= 0xff;
                std::swap(perm[index1], perm[index2]);
                index3 = perm[index1] + perm[index2];
                index3 &= 0xff;
                result.push_back((input[i] ^ perm[index3]) & 0xff);
            }

            return result;
        }

        template<typename ContainerT>
        std::vector<std::uint8_t> mix_a(
            const ContainerT& data, const std::uint16_t product_id)
        {
            std::vector<std::uint8_t> result(8);

            result[0] = data[0];
            result[1] = data[2];
            result[2] = data[5];
            result[3] = product_id & 0xff;
            result[4] = product_id & 0xff;
            result[5] = data[4];
            result[6] = data[5];
            result[7] = data[1];

            return result;
        }

        template<typename ContainerT>
        std::vector<std::uint8_t> mix_b(
            const ContainerT& data, const std::uint16_t product_id)
        {
            std::vector<std::uint8_t> result(8);

            result[0] = data[0];
            result[1] = data[2];
            result[2] = data[5];
            result[3] = (product_id >> 8) & 0xff;
            result[4] = data[4];
            result[5] = data[0];
            result[6] = data[5];
            result[7] = product_id & 0xff;

            return result;
        }

        inline std::string_view action_to_sv(const std::uint8_t action) noexcept
        {
            switch (action)
            {
            case 0U:
                return ACTION_IDLE;
            case 1U:
                return ACTION_HEATING;
            case 2U:
                return ACTION_COOLING;
            case 3U:
                return ACTION_KEEPING_WARM;
            default:
                assert(0);
                std::abort();
            }
        }

        inline std::uint8_t sv_to_action(const std::string_view action) noexcept
        {
            using lookup_t =
                utils::const_map<std::string_view, const std::uint8_t, 4>;

            static constexpr lookup_t action_lookup{ {
                { ACTION_IDLE, 0U },
                { ACTION_HEATING, 1U },
                { ACTION_COOLING, 2U },
                { ACTION_KEEPING_WARM, 3U },
            } };

            return action_lookup[action];
        }

        inline std::string_view mode_to_sv(const std::uint8_t mode) noexcept
        {
            switch (mode)
            {
            case 1U:
                return MODE_BOIL;
            case 2U:
                return MODE_WARM;
            case 255U:
                return MODE_NONE;
            default:
                assert(0);
                std::abort();
            }
        }

        inline std::uint8_t sv_to_mode(const std::string_view mode) noexcept
        {
            using lookup_t =
                utils::const_map<std::string_view, const std::uint8_t, 3>;

            static constexpr lookup_t mode_lookup{ {
                { MODE_BOIL, 1U },
                { MODE_WARM, 2U },
                { MODE_NONE, 255U },
            } };

            return mode_lookup[mode];
        }

        inline std::string_view keep_warm_type_to_sv(
            const std::uint8_t keep_warm_type) noexcept
        {
            switch (keep_warm_type)
            {
            case 0U:
                return KEEP_WARM_TYPE_BOIL_AND_COOL;
            case 1U:
                return KEEP_WARM_TYPE_HEAT_UP;
            default:
                assert(0);
                std::abort();
            }
        }

        inline std::uint8_t sv_to_keep_warm_type(
            const std::string_view mode) noexcept
        {
            using lookup_t =
                utils::const_map<std::string_view, const std::uint8_t, 2>;

            static constexpr lookup_t type_lookup{ {
                { KEEP_WARM_TYPE_BOIL_AND_COOL, 0U },
                { KEEP_WARM_TYPE_HEAT_UP, 1U },
            } };

            return type_lookup[mode];
        }

        inline std::string_view uint8_to_switch_val(
            const std::uint8_t val) noexcept
        {
            using namespace std::literals;

            switch (val)
            {
            case 0U:
                return "OFF"sv;
            case 1U:
                return "ON"sv;
            default:
                assert(0);
                std::abort();
            }
        }

        inline std::uint8_t switch_val_to_uint8(
            const std::string_view val) noexcept
        {
            using namespace std::literals;

            using lookup_t =
                utils::const_map<std::string_view, const std::uint8_t, 2>;

            static constexpr lookup_t val_lookup{ {
                { "OFF"sv, 0U },
                { "ON"sv, 1U },
            } };

            return val_lookup[val];
        }

        struct mikettle_state {
            using external_event_variant_t = std::variant<events::abort,
                events::srv_disced, events::chrs_disced, events::descs_disced,
                events::read_finished, events::write_finished,
                events::mqtt_data, events::sub_finished>;

            struct cache_t {
                std::uint8_t action = std::numeric_limits<std::uint8_t>::max();
                std::uint8_t mode   = std::numeric_limits<std::uint8_t>::max();
                std::uint8_t temperature_set =
                    std::numeric_limits<std::uint8_t>::max();
                std::uint8_t temperature_current =
                    std::numeric_limits<std::uint8_t>::max();
                std::uint8_t keep_warm_type =
                    std::numeric_limits<std::uint8_t>::max();
                std::uint16_t keep_warm_time =
                    std::numeric_limits<std::uint16_t>::max();
                std::uint8_t keep_warm_time_limit =
                    std::numeric_limits<std::uint8_t>::max();
                std::uint8_t turn_off_after_boil =
                    std::numeric_limits<std::uint8_t>::max();
            };

            struct connecting {
                std::uint16_t auth_init_chr_handle = 0;
                std::uint16_t auth_chr_handle      = 0;
                std::uint16_t version_chr_handle   = 0;
                ble::gatt::characteristic auth_chr = {};
            };

            struct operating {
                std::uint16_t status_chr_handle    = 0;
                std::uint16_t time_chr_handle      = 0;
                std::uint16_t setup_chr_handle     = 0;
                std::uint16_t boil_mode_chr_handle = 0;

                std::uint16_t temp_val = 0;

                cache_t cache = {};
            };

            using state_variant_t = std::variant<connecting, operating>;

            ble::gatt::client& gatt_client;
            mqtt::client& mqtt_client;

            state_variant_t state_var;

            std::function<void(external_event_variant_t)>
                process_external_event;
        };

        struct mikettle_fsm {
            auto operator()() noexcept
            {
                namespace sml = boost::sml;

                using namespace sml;

                const auto find_by_uuid = [](const auto& cont,
                                              const auto& uuid) {
                    return std::find_if(cont.begin(),
                        cont.end(),
                        [&](const auto& attr) {
                            return ::ble_uuid_cmp(&attr.uuid.u, &uuid.u) == 0;
                        });
                };

                const auto disc_srv_handler = [](mikettle_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value() || result.value().empty())
                        {
                            log::error(COMPONENT, "Service discovery failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        state.process_external_event(
                            events::srv_disced{ result.value() });
                    };
                };

                const auto disc_chr_handler = [](mikettle_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value() || result.value().empty())
                        {
                            log::error(COMPONENT,
                                "Characteristic discovery failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        state.process_external_event(
                            events::chrs_disced{ result.value() });
                    };
                };

                const auto disc_desc_handler = [](mikettle_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value() || result.value().empty())
                        {
                            log::error(COMPONENT,
                                "Descriptor discovery failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        state.process_external_event(
                            events::descs_disced{ result.value() });
                    };
                };

                const auto write_handler = [](mikettle_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value())
                        {
                            log::warning(COMPONENT, "Write failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        state.process_external_event(events::write_finished{});
                    };
                };

                const auto subscribe_handler = [](mikettle_state& state) {
                    return [&](auto&& result) {
                        if (!result.has_value())
                        {
                            log::warning(COMPONENT, "Subscribe failed.");
                            state.process_external_event(events::abort{});
                            return;
                        }

                        state.process_external_event(events::sub_finished{});
                    };
                };

                const auto mqtt_sub = [=](const char* topic) {
                    return [=](mikettle_state& state) {
                        state.mqtt_client.async_subscribe(topic,
                            1,
                            subscribe_handler(state));
                    };
                };

                const auto on_start = [=](mikettle_state& state) {
                    state.gatt_client.async_discover_service_by_uuid(
                        &GATT_UUID_KETTLE_SRV.u,
                        disc_srv_handler(state));
                };

                const auto on_srv_disced = [=](mikettle_state& state,
                                               events::srv_disced event) {
                    state.gatt_client.async_discover_characteristics(
                        event.services.front(),
                        disc_chr_handler(state));
                };

                const auto on_kettle_chrs =
                    [=](mikettle_state& state,
                        const events::chrs_disced& event,
                        back::process<events::abort>
                            back_process) {
                        const auto auth_init_it =
                            find_by_uuid(event.characteristics,
                                GATT_UUID_AUTH_INIT);
                        if (auth_init_it == event.characteristics.end())
                        {
                            log::error(COMPONENT,
                                "AUTH INIT characteristic not found.");
                            back_process(events::abort{});
                            return;
                        }

                        const auto auth_it =
                            find_by_uuid(event.characteristics, GATT_UUID_AUTH);
                        if (auth_it == event.characteristics.end())
                        {
                            log::error(COMPONENT,
                                "AUTH characteristic not found.");
                            back_process(events::abort{});
                            return;
                        }

                        const auto ver_it = find_by_uuid(event.characteristics,
                            GATT_UUID_VERSION);
                        if (ver_it == event.characteristics.end())
                        {
                            log::error(COMPONENT,
                                "VERSION characteristic not found.");
                            back_process(events::abort{});
                            return;
                        }

                        auto& state_var = std::get<mikettle_state::connecting>(
                            state.state_var);

                        state_var.auth_init_chr_handle =
                            auth_init_it->val_handle;
                        state_var.auth_chr           = *auth_it;
                        state_var.auth_chr_handle    = auth_it->val_handle;
                        state_var.version_chr_handle = ver_it->val_handle;
                    };

                const auto on_auth_start = [=](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::connecting>(state.state_var);
                    state.gatt_client.async_write(
                        state_var.auth_init_chr_handle,
                        key1,
                        write_handler(state));
                };

                const auto on_auth_init_write = [=](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::connecting>(state.state_var);
                    state.gatt_client.async_discover_descriptors(
                        state_var.auth_chr,
                        disc_desc_handler(state));
                };

                const auto on_cccd = [=](mikettle_state& state,
                                         const events::descs_disced& event,
                                         back::process<events::abort>
                                             back_process) {
                    const auto cccd_it =
                        find_by_uuid(event.descriptors, GATT_UUID_CCCD);
                    if (cccd_it == event.descriptors.end())
                    {
                        log::error(COMPONENT,
                            "CCCD characteristic descriptor not found.");
                        back_process(events::abort{});
                        return;
                    }

                    state.gatt_client.async_write(cccd_it->handle,
                        subscribe,
                        write_handler(state));
                };

                const auto on_auth_subscribe = [=](mikettle_state& state) {
                    auto reversed_mac{
                        utils::make_mac(std::make_reverse_iterator(
                                            state.gatt_client.mac().as_bytes() +
                                            utils::mac::MAC_SIZE),
                            std::make_reverse_iterator(
                                state.gatt_client.mac().as_bytes()))
                            .value(),
                    };

                    auto& state_var =
                        std::get<mikettle_state::connecting>(state.state_var);

                    state.gatt_client.async_write(state_var.auth_chr_handle,
                        cipher(mix_a(state.gatt_client.mac().as_bytes(),
                                   PRODUCT_ID),
                            token),
                        write_handler(state));
                };

                const auto on_auth_notify = [](mikettle_state& state,
                                                const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::connecting>(state.state_var);

                    if (event.attr_handle != state_var.auth_chr_handle)
                    {
                        return;
                    }

                    state.gatt_client.async_write(state_var.auth_chr_handle,
                        cipher(token, key2),
                        [&](auto&&) {
                            state.gatt_client.async_read(
                                state_var.version_chr_handle,
                                event::empty_handler); // Data must be read but
                                                       // can be ignored.

                            state.gatt_client.async_write(
                                state_var.auth_chr_handle,
                                unsubscribe,
                                [&](auto&&) {
                                    state.state_var =
                                        mikettle_state::operating{};
                                });
                        });
                };

                const auto on_conf_temp_sens = [=](mikettle_state& state) {
                    hass::sensor_type sens;

                    sens.name                = TEMPERATURE_SENSOR_NAME;
                    sens.state_topic         = TEMPERATURE_SENSOR_STATE_TOPIC;
                    sens.device_class        = "temperature";
                    sens.unit_of_measurement = "°C";
                    sens.qos                 = 1;

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_actn_sens = [=](mikettle_state& state) {
                    hass::sensor_type sens;

                    sens.name        = ACTION_SENSOR_NAME;
                    sens.state_topic = ACTION_SENSOR_STATE_TOPIC;
                    sens.qos         = 1;

                    state.mqtt_client.async_publish(ACTION_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_mode_sens = [=](mikettle_state& state) {
                    hass::sensor_type sens;

                    sens.name        = MODE_SENSOR_NAME;
                    sens.state_topic = MODE_SENSOR_STATE_TOPIC;
                    sens.qos         = 1;

                    state.mqtt_client.async_publish(MODE_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_warm_time_sens = [=](mikettle_state& state) {
                    hass::sensor_type sens;

                    sens.name        = KEEP_WARM_TIME_SENSOR_NAME;
                    sens.state_topic = KEEP_WARM_TIME_SENSOR_STATE_TOPIC;
                    sens.unit_of_measurement = "min";
                    sens.qos                 = 1;

                    state.mqtt_client.async_publish(
                        KEEP_WARM_TIME_SENSOR_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sens)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_temp_set_num = [=](mikettle_state& state) {
                    hass::number_type num;

                    num.name          = TEMPERATURE_SET_NUMBER_NAME;
                    num.state_topic   = TEMPERATURE_SET_NUMBER_STATE_TOPIC;
                    num.command_topic = TEMPERATURE_SET_NUMBER_CMD_TOPIC;
                    num.min           = 40.0;
                    num.max           = 95.0;
                    num.step          = 1.0;
                    num.unit_of_measurement = "°C";
                    num.retain              = true;
                    num.qos                 = 1;

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SET_NUMBER_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(num)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_warm_limit_num = [=](mikettle_state& state) {
                    hass::number_type num;

                    num.name          = KEEP_WARM_TIME_LIMIT_NUMBER_NAME;
                    num.state_topic   = KEEP_WARM_TIME_LIMIT_NUMBER_STATE_TOPIC;
                    num.command_topic = KEEP_WARM_TIME_LIMIT_NUMBER_CMD_TOPIC;
                    num.min           = 1.0;
                    num.max           = 12.0;
                    num.step          = 0.5;
                    num.unit_of_measurement = "h";
                    num.retain              = true;
                    num.qos                 = 1;

                    state.mqtt_client.async_publish(
                        KEEP_WARM_TIME_LIMIT_NUMBER_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(num)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_warm_type = [=](mikettle_state& state) {
                    using namespace std::literals;

                    hass::select_type sel;

                    std::array opts{
                        KEEP_WARM_TYPE_BOIL_AND_COOL,
                        KEEP_WARM_TYPE_HEAT_UP,
                    };

                    sel.name          = KEEP_WARM_TYPE_SELECT_NAME;
                    sel.command_topic = KEEP_WARM_TYPE_SELECT_CMD_TOPIC;
                    sel.state_topic   = KEEP_WARM_TYPE_SELECT_STATE_TOPIC;
                    sel.options       = tcb::make_span(opts);
                    sel.retain        = true;
                    sel.qos           = 1;

                    state.mqtt_client.async_publish(
                        KEEP_WARM_TYPE_SELECT_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sel)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_conf_toab_sel = [=](mikettle_state& state) {
                    hass::switch_type sw;

                    sw.name          = TURN_OFF_AFTER_BOIL_SWITCH_NAME;
                    sw.state_topic   = TURN_OFF_AFTER_BOIL_SWITCH_STATE_TOPIC;
                    sw.command_topic = TURN_OFF_AFTER_BOIL_SWITCH_CMD_TOPIC;
                    sw.retain        = true;
                    sw.qos           = 1;

                    state.mqtt_client.async_publish(
                        TURN_OFF_AFTER_BOIL_SWITCH_CONFIG_TOPIC,
                        utils::json::dump(hass::serialize(sw)),
                        1,
                        true,
                        write_handler(state));
                };

                const auto on_disc_data_srv = [=](mikettle_state& state) {
                    state.gatt_client.async_discover_service_by_uuid(
                        &GATT_UUID_KETTLE_DATA_SRV.u,
                        disc_srv_handler(state));
                };

                const auto on_data_chrs = [=](mikettle_state& state,
                                              const events::chrs_disced& event,
                                              back::process<events::abort>
                                                  back_process) {
                    const auto status_it =
                        find_by_uuid(event.characteristics, GATT_UUID_STATUS);
                    if (status_it == event.characteristics.end())
                    {
                        log::error(COMPONENT,
                            "STATUS characteristic not found.");
                        back_process(events::abort{});
                        return;
                    }

                    const auto time_it =
                        find_by_uuid(event.characteristics, GATT_UUID_TIME);
                    if (time_it == event.characteristics.end())
                    {
                        log::error(COMPONENT, "TIME characteristic not found.");
                        back_process(events::abort{});
                        return;
                    }

                    const auto setup_it =
                        find_by_uuid(event.characteristics, GATT_UUID_SETUP);
                    if (setup_it == event.characteristics.end())
                    {
                        log::error(COMPONENT,
                            "SETUP characteristic not found.");
                        back_process(events::abort{});
                        return;
                    }

                    const auto boil_mode_it =
                        find_by_uuid(event.characteristics,
                            GATT_UUID_BOIL_MODE);
                    if (boil_mode_it == event.characteristics.end())
                    {
                        log::error(COMPONENT,
                            "BOIL MODE characteristic not found.");
                        back_process(events::abort{});
                        return;
                    }

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.status_chr_handle    = status_it->val_handle;
                    state_var.time_chr_handle      = time_it->val_handle;
                    state_var.setup_chr_handle     = setup_it->val_handle;
                    state_var.boil_mode_chr_handle = boil_mode_it->val_handle;

                    state.gatt_client.async_discover_descriptors(*status_it,
                        disc_desc_handler(state));
                };

                const auto sub_temp_set =
                    mqtt_sub(TEMPERATURE_SET_NUMBER_CMD_TOPIC);

                const auto sub_warm_time_limit =
                    mqtt_sub(KEEP_WARM_TIME_LIMIT_NUMBER_CMD_TOPIC);

                const auto sub_warm_type =
                    mqtt_sub(KEEP_WARM_TYPE_SELECT_CMD_TOPIC);

                const auto sub_toab =
                    mqtt_sub(TURN_OFF_AFTER_BOIL_SWITCH_CMD_TOPIC);

                const auto time_set_start_read = [](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_read(state_var.time_chr_handle,
                        [&](auto&& result) {
                            if (!result.has_value())
                            {
                                log::warning(COMPONENT,
                                    "Failed to read TIME characteristic.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            assert(result.value().size() != 0);

                            state_var.cache.keep_warm_time_limit =
                                result.value().front();

                            state.process_external_event(
                                events::read_finished{});
                        });
                };

                const auto toab_start_read = [](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_read(state_var.boil_mode_chr_handle,
                        [&](auto&& result) {
                            if (!result.has_value())
                            {
                                log::warning(COMPONENT,
                                    "Failed to read BOIL MODE characteristic.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            assert(result.value().size() != 0);

                            state_var.cache.turn_off_after_boil =
                                result.value().front();

                            state.process_external_event(
                                events::read_finished{});
                        });
                };

                const auto is_status_chr = [](mikettle_state& state,
                                               const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    return event.attr_handle == state_var.status_chr_handle;
                };

                const auto is_actn_upd = [](mikettle_state& state,
                                             const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    return state_var.cache.action != event.data[0];
                };

                const auto upd_actn = [=](mikettle_state& state,
                                          const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);
                    state_var.cache.action = event.data[0];

                    state.mqtt_client.async_publish(ACTION_SENSOR_STATE_TOPIC,
                        action_to_sv(event.data[0]),
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_mode_upd = [](mikettle_state& state,
                                             const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);
                    return state_var.cache.mode != event.data[1];
                };

                const auto upd_mode = [=](mikettle_state& state,
                                          const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.cache.mode = event.data[1];

                    state.mqtt_client.async_publish(MODE_SENSOR_STATE_TOPIC,
                        mode_to_sv(event.data[1]),
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_temp_set_upd = [](mikettle_state& state,
                                                 const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);
                    return state_var.cache.temperature_set != event.data[4];
                };

                const auto upd_temp_set = [=](mikettle_state& state,
                                              const events::notify& event) {
                    using buff_t = std::array<char, 4>;

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.cache.temperature_set = event.data[4];

                    buff_t buff;
                    buff_t::iterator iter;

                    iter = fmt::format_to(buff.begin(),
                        "{}",
                        state_var.cache.temperature_set);

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SET_NUMBER_STATE_TOPIC,
                        std::string_view{
                            buff.data(),
                            static_cast<std::size_t>(iter - buff.begin()),
                        },
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_temp_upd = [](mikettle_state& state,
                                             const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);
                    return state_var.cache.temperature_current != event.data[5];
                };

                const auto upd_temp = [=](mikettle_state& state,
                                          const events::notify& event) {
                    using buff_t = std::array<char, 4>;

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.cache.temperature_current = event.data[5];

                    buff_t buff;
                    buff_t::iterator iter;

                    iter = fmt::format_to(buff.begin(),
                        "{}",
                        state_var.cache.temperature_current);

                    state.mqtt_client.async_publish(
                        TEMPERATURE_SENSOR_STATE_TOPIC,
                        std::string_view{
                            buff.data(),
                            static_cast<std::size_t>(iter - buff.begin()),
                        },
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_warm_type_upd = [](mikettle_state& state,
                                                  const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);
                    return state_var.cache.keep_warm_type != event.data[6];
                };

                const auto upd_warm_type = [=](mikettle_state& state,
                                               const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.cache.keep_warm_type = event.data[6];

                    state.mqtt_client.async_publish(
                        KEEP_WARM_TYPE_SELECT_STATE_TOPIC,
                        keep_warm_type_to_sv(state_var.cache.keep_warm_type),
                        1,
                        true,
                        write_handler(state));
                };

                const auto is_warm_time_upd = [](mikettle_state& state,
                                                  const events::notify& event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.temp_val = (event.data[7] << 8) & event.data[8];

                    return state_var.cache.keep_warm_time != state_var.temp_val;
                };

                const auto upd_warm_time = [=](mikettle_state& state,
                                               const events::notify& event) {
                    using buff_t = std::array<char, 8>;

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state_var.cache.keep_warm_time = state_var.temp_val;

                    buff_t buff;
                    buff_t::iterator iter;

                    iter = fmt::format_to(buff.begin(),
                        "{}",
                        state_var.cache.keep_warm_time);

                    state.mqtt_client.async_publish(
                        KEEP_WARM_TIME_SENSOR_STATE_TOPIC,
                        std::string_view{
                            buff.data(),
                            static_cast<std::size_t>(iter - buff.begin()),
                        },
                        1,
                        true,
                        write_handler(state));
                };

                const auto mqtt_receive = [](mikettle_state& state) {
                    const auto mqtt_receive_impl = [](auto self,
                                                       mikettle_state& state) {
                        state.mqtt_client.async_receive([&, self](
                                                            auto&& result) {
                            if (!result.has_value())
                            {
                                log::warning(COMPONENT,
                                    "Failed to read MQTT data.");
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state.process_external_event(events::mqtt_data{
                                result.value().topic,
                                result.value().data,
                            });

                            self(self, state);
                        });
                    };

                    mqtt_receive_impl(mqtt_receive_impl, state);
                };

                const auto make_topic_guard = [](const std::string_view topic) {
                    return [=](events::mqtt_data event) {
                        return topic == event.topic;
                    };
                };

                const auto topic_warm_type =
                    make_topic_guard(KEEP_WARM_TYPE_SELECT_CMD_TOPIC);

                const auto is_warm_type_mqtt_upd =
                    [](mikettle_state& state, events::mqtt_data event) {
                        const std::uint8_t keep_warm_type =
                            sv_to_keep_warm_type(event.data);

                        auto& state_var = std::get<mikettle_state::operating>(
                            state.state_var);

                        if (keep_warm_type == state_var.cache.keep_warm_type)
                        {
                            return false;
                        }

                        state_var.temp_val =
                            static_cast<std::uint16_t>(keep_warm_type);

                        return true;
                    };

                const auto warm_type_write = [=](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_write(state_var.setup_chr_handle,
                        std::array{
                            static_cast<std::uint8_t>(state_var.temp_val),
                            state_var.cache.temperature_set,
                        },
                        write_handler(state));
                };

                const auto topic_temp_set =
                    make_topic_guard(TEMPERATURE_SET_NUMBER_CMD_TOPIC);

                const auto is_temp_set_mqtt_upd = [](mikettle_state& state,
                                                      events::mqtt_data event) {
                    std::uint8_t temperature_set = 0U;
                    if (const auto [ptr, ec] =
                            std::from_chars(event.data.data(),
                                event.data.data() + event.data.size(),
                                temperature_set);
                        ec != std::errc{})
                    {
                        log::warning(COMPONENT,
                            "Temperature conversion failed.");
                        return false;
                    }

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    if (temperature_set == state_var.cache.temperature_set)
                    {
                        return false;
                    }

                    state_var.temp_val =
                        static_cast<std::uint16_t>(temperature_set);

                    return true;
                };

                const auto temp_set_write = [=](mikettle_state& state,
                                                events::mqtt_data event) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_write(state_var.setup_chr_handle,
                        std::array{
                            state_var.cache.keep_warm_type,
                            static_cast<std::uint8_t>(state_var.temp_val),
                        },
                        write_handler(state));
                };

                const auto topic_warm_time_limit =
                    make_topic_guard(KEEP_WARM_TIME_LIMIT_NUMBER_CMD_TOPIC);

                const auto is_warm_time_limit_mqtt_upd =
                    [](mikettle_state& state, events::mqtt_data event) {
                        const std::uint8_t time_limit =
                            static_cast<std::uint8_t>(
                                2.0f * std::stof(std::string{ event.data }));

                        auto& state_var = std::get<mikettle_state::operating>(
                            state.state_var);

                        if (time_limit == state_var.cache.keep_warm_time_limit)
                        {
                            return false;
                        }

                        state_var.temp_val =
                            static_cast<std::uint16_t>(time_limit);

                        return true;
                    };

                const auto warm_time_limit_write = [=](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_write(state_var.time_chr_handle,
                        std::array{
                            static_cast<std::uint8_t>(state_var.temp_val),
                        },
                        [&, write_handler](auto&& result) {
                            using buff_t = std::array<char, 4>;

                            if (!result.has_value())
                            {
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state_var.cache.keep_warm_time_limit =
                                state_var.temp_val;

                            buff_t buff;
                            buff_t::iterator iter;

                            iter = fmt::format_to(buff.begin(),
                                "{:.1f}",
                                (static_cast<float>(
                                     state_var.cache.keep_warm_time_limit) /
                                    2.0f));

                            state.mqtt_client.async_publish(
                                KEEP_WARM_TIME_LIMIT_NUMBER_STATE_TOPIC,
                                std::string_view{
                                    buff.data(),
                                    static_cast<std::size_t>(
                                        iter - buff.begin()),
                                },
                                1,
                                true,
                                write_handler(state));
                        });
                };

                const auto topic_toab =
                    make_topic_guard(TURN_OFF_AFTER_BOIL_SWITCH_CMD_TOPIC);

                const auto is_toab_mqtt_upd = [](mikettle_state& state,
                                                  events::mqtt_data event) {
                    const std::uint8_t turn_off_after_boil =
                        switch_val_to_uint8(event.data);

                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    if (turn_off_after_boil ==
                        state_var.cache.turn_off_after_boil)
                    {
                        return false;
                    }

                    state_var.temp_val =
                        static_cast<std::uint16_t>(turn_off_after_boil);

                    return true;
                };

                const auto toab_write = [=](mikettle_state& state) {
                    auto& state_var =
                        std::get<mikettle_state::operating>(state.state_var);

                    state.gatt_client.async_write(
                        state_var.boil_mode_chr_handle,
                        std::array{
                            static_cast<std::uint8_t>(state_var.temp_val),
                        },
                        [&, write_handler](auto&& result) {
                            if (!result.has_value())
                            {
                                state.process_external_event(events::abort{});
                                return;
                            }

                            state_var.cache.turn_off_after_boil =
                                state_var.temp_val;

                            state.mqtt_client.async_publish(
                                TURN_OFF_AFTER_BOIL_SWITCH_STATE_TOPIC,
                                uint8_to_switch_val(state_var.temp_val),
                                1,
                                true,
                                write_handler(state));
                        });
                };

                const auto on_abort_conn = [](mikettle_state& state) {
                    log::warning(COMPONENT, "Terminating BLE connection.");
                    state.gatt_client.terminate();
                };

                // clang-format off
                return make_transition_table(
                    *"init"_s               + sml::event<events::connected>      / on_start                    = "disc_kettle_srv"_s,

                    "disc_kettle_srv"_s     + sml::event<events::srv_disced>     / on_srv_disced               = "disc_kettle_chrs"_s,
                    "disc_kettle_srv"_s     + sml::event<events::abort>                                        = "terminate"_s,

                    "disc_kettle_chrs"_s    + sml::event<events::chrs_disced>    / on_kettle_chrs              = "auth_start"_s,
                    "disc_kettle_chrs"_s    + sml::event<events::abort>                                        = "terminate"_s,

                    "auth_start"_s          + on_entry<_>                        / on_auth_start,
                    "auth_start"_s          + sml::event<events::write_finished> / on_auth_init_write          = "disc_auth_cccd"_s,
                    "auth_start"_s          + sml::event<events::abort>                                        = "terminate"_s,

                    "disc_auth_cccd"_s      + sml::event<events::descs_disced>   / on_cccd                     = "auth_subscribed"_s,
                    "disc_auth_cccd"_s      + sml::event<events::abort>                                        = "terminate"_s,

                    "auth_subscribed"_s     + sml::event<events::write_finished> / on_auth_subscribe           = "wait_auth_notify"_s,
                    "auth_subscribed"_s     + sml::event<events::abort>                                        = "terminate"_s,

                    "wait_auth_notify"_s    + sml::event<events::notify>         / on_auth_notify              = "conf_temp_sens"_s,
                    "wait_auth_notify"_s    + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_temp_sens"_s      + on_entry<_>                        / on_conf_temp_sens,
                    "conf_temp_sens"_s      + sml::event<events::write_finished>                               = "conf_actn_sens"_s,
                    "conf_temp_sens"_s      + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_actn_sens"_s      + on_entry<_>                        / on_conf_actn_sens,
                    "conf_actn_sens"_s      + sml::event<events::write_finished>                               = "conf_mode_sens"_s,
                    "conf_actn_sens"_s      + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_mode_sens"_s      + on_entry<_>                        / on_conf_mode_sens,
                    "conf_mode_sens"_s      + sml::event<events::write_finished>                               = "conf_warm_time_sens"_s,
                    "conf_mode_sens"_s      + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_warm_time_sens"_s + on_entry<_>                        / on_conf_warm_time_sens,
                    "conf_warm_time_sens"_s + sml::event<events::write_finished>                               = "conf_temp_set_num"_s,
                    "conf_warm_time_sens"_s + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_temp_set_num"_s   + on_entry<_>                        / on_conf_temp_set_num,
                    "conf_temp_set_num"_s   + sml::event<events::write_finished>                               = "conf_warm_limit_num"_s,
                    "conf_temp_set_num"_s   + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_warm_limit_num"_s + on_entry<_>                        / on_conf_warm_limit_num,
                    "conf_warm_limit_num"_s + sml::event<events::write_finished>                               = "conf_warm_type_sel"_s,
                    "conf_warm_limit_num"_s + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_warm_type_sel"_s  + on_entry<_>                        / on_conf_warm_type,
                    "conf_warm_type_sel"_s  + sml::event<events::write_finished>                               = "conf_toab_sel"_s,
                    "conf_warm_type_sel"_s  + sml::event<events::abort>                                        = "terminate"_s,

                    "conf_toab_sel"_s       + on_entry<_>                        / on_conf_toab_sel,
                    "conf_toab_sel"_s       + sml::event<events::write_finished> / on_disc_data_srv            = "disc_data_srv"_s,
                    "conf_toab_sel"_s       + sml::event<events::abort>                                        = "terminate"_s,

                    "disc_data_srv"_s       + sml::event<events::srv_disced>     / on_srv_disced               = "disc_status_chrs"_s,
                    "disc_data_srv"_s       + sml::event<events::abort>                                        = "terminate"_s,

                    "disc_status_chrs"_s    + sml::event<events::chrs_disced>    / on_data_chrs                = "disc_status_cccd"_s,
                    "disc_status_chrs"_s    + sml::event<events::abort>                                        = "terminate"_s,

                    "disc_status_cccd"_s    + sml::event<events::descs_disced>   / on_cccd                     = "read_time_set"_s,
                    "disc_status_cccd"_s    + sml::event<events::abort>                                        = "terminate"_s,

                    "read_time_set"_s       + on_entry<_>                        / time_set_start_read,
                    "read_time_set"_s       + sml::event<events::read_finished>                                = "read_toab"_s,
                    "read_time_set"_s       + sml::event<events::abort>                                        = "terminate"_s,

                    "read_toab"_s           + on_entry<_>                        / toab_start_read,
                    "read_toab"_s           + sml::event<events::read_finished>  / sub_temp_set                = "sub_temp_set"_s,
                    "read_toab"_s           + sml::event<events::abort>                                        = "terminate"_s,


                    "sub_temp_set"_s        + sml::event<events::sub_finished>   / sub_warm_time_limit         = "sub_warm_time"_s,
                    "sub_temp_set"_s        + sml::event<events::abort>                                        = "terminate"_s,

                    
                    "sub_warm_time"_s       + sml::event<events::sub_finished>   / sub_warm_type               = "sub_warm_type"_s,
                    "sub_warm_time"_s       + sml::event<events::abort>                                        = "terminate"_s,
          
                    "sub_warm_type"_s       + sml::event<events::sub_finished>   / sub_toab                    = "sub_toab"_s,
                    "sub_warm_type"_s       + sml::event<events::abort>                                        = "terminate"_s,

                    "sub_toab"_s            + sml::event<events::sub_finished>   / mqtt_receive                = "operate"_s,
                    "sub_toab"_s            + sml::event<events::abort>                                        = "terminate"_s,

                    // Configuration is finished, start normal operation.
                    
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_temp_set_upd]  / upd_temp_set  = "param_write"_s,
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_warm_type_upd] / upd_warm_type = "param_write"_s,
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_warm_time_upd] / upd_warm_time = "param_write"_s,
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_actn_upd]      / upd_actn      = "param_write"_s,
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_mode_upd]      / upd_mode      = "param_write"_s,          
                    "operate"_s + sml::event<events::notify> [is_status_chr && is_temp_upd]      / upd_temp      = "param_write"_s,
    
                    "operate"_s + sml::event<events::mqtt_data> [topic_warm_type && is_warm_type_mqtt_upd]             / warm_type_write       = "param_write"_s,
                    "operate"_s + sml::event<events::mqtt_data> [topic_temp_set && is_temp_set_mqtt_upd]               / temp_set_write        = "param_write"_s,
                    "operate"_s + sml::event<events::mqtt_data> [topic_toab && is_toab_mqtt_upd]                       / toab_write            = "param_write"_s,
                    "operate"_s + sml::event<events::mqtt_data> [topic_warm_time_limit && is_warm_time_limit_mqtt_upd] / warm_time_limit_write = "param_write"_s,

                    "operate"_s + sml::event<events::abort>         = "terminate"_s,
                    "operate"_s + sml::event<events::disconnected>  = X,

                    "param_write"_s + sml::event<events::write_finished> = "operate"_s,
                    "param_write"_s + sml::event<events::abort>          = "operate"_s,
                    "param_write"_s + sml::event<events::disconnected>   = X,

                    "terminate"_s + on_entry<_> / on_abort_conn
                );
                // clang-format on
            }
        };

    } // namespace mikettle_impl

    class mikettle final : public base
    {
    public:
        mikettle(std::unique_ptr<mqtt::client>&& mqtt_client,
            std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept;

        mikettle()                = delete;
        mikettle(const mikettle&) = delete;
        mikettle(mikettle&&)      = default;
        ~mikettle()               = default;

        mikettle& operator=(const mikettle&) = delete;
        mikettle& operator=(mikettle&&) = default;

        void on_connected() noexcept override;

        void on_notify(std::uint16_t attribute_handle,
            std::vector<std::uint8_t>&& data) noexcept override;

    private:
        using fsm_t = boost::sml::sm<mikettle_impl::mikettle_fsm,
            boost::sml::thread_safe<std::recursive_mutex>,
            boost::sml::process_queue<std::queue>>;

        mikettle_impl::mikettle_state m_state;
        fsm_t m_fsm;

        auto make_process_external_event()
        {
            using external_event_variant_t =
                mikettle_impl::mikettle_state::external_event_variant_t;
            return [this](const external_event_variant_t& event_variant) {
                std::visit(
                    [this](const auto& arg) { m_fsm.process_event(arg); },
                    event_variant);
            };
        }
    };
} // namespace b2h::device::xiaomi

#endif
