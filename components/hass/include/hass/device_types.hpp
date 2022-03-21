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

#ifndef B2H_HASS_DEVICE_TYPES_HPP
#define B2H_HASS_DEVICE_TYPES_HPP

#include "device_type.hpp"
#include "rapidjson/document.h"

namespace b2h::hass
{
    // Autogenerated from
    // https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/.
    struct alarm_control_panel_type {
        std::string_view command_topic;
        std::string_view state_topic;
        std::optional<bool> code_arm_required;
        std::optional<bool> code_disarm_required;
        std::optional<bool> enabled_by_default;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> code;
        std::optional<std::string_view> command_template;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_arm_away;
        std::optional<std::string_view> payload_arm_custom_bypass;
        std::optional<std::string_view> payload_arm_home;
        std::optional<std::string_view> payload_arm_night;
        std::optional<std::string_view> payload_arm_vacation;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_disarm;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/binary_sensor.mqtt/.
    struct binary_sensor_type {
        std::string_view state_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> force_update;
        std::optional<device_type*> device;
        std::optional<int> expire_after;
        std::optional<int> off_delay;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> device_class;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_off;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/camera.mqtt/.
    struct camera_type {
        std::string_view topic;
        std::optional<bool> enabled_by_default;
        std::optional<device_type*> device;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> unique_id;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/cover.mqtt/.
    struct cover_type {
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<bool> tilt_optimistic;
        std::optional<device_type*> device;
        std::optional<int> position_closed;
        std::optional<int> position_open;
        std::optional<int> qos;
        std::optional<int> tilt_closed_value;
        std::optional<int> tilt_max;
        std::optional<int> tilt_min;
        std::optional<int> tilt_opened_value;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_topic;
        std::optional<std::string_view> device_class;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_close;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_open;
        std::optional<std::string_view> payload_stop;
        std::optional<std::string_view> position_template;
        std::optional<std::string_view> position_topic;
        std::optional<std::string_view> set_position_template;
        std::optional<std::string_view> set_position_topic;
        std::optional<std::string_view> state_closed;
        std::optional<std::string_view> state_closing;
        std::optional<std::string_view> state_open;
        std::optional<std::string_view> state_opening;
        std::optional<std::string_view> state_stopped;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> tilt_command_template;
        std::optional<std::string_view> tilt_command_topic;
        std::optional<std::string_view> tilt_status_template;
        std::optional<std::string_view> tilt_status_topic;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/device_tracker.mqtt/.
    struct device_tracker_type {
        tcb::span<std::string_view> devices;
        std::optional<int> qos;
        std::optional<std::string_view> payload_home;
        std::optional<std::string_view> payload_not_home;
        std::optional<std::string_view> source_type;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/device_trigger.mqtt/.
    struct device_trigger_type {
        device_type device;
        std::string_view automation_type;
        std::string_view subtype;
        std::string_view topic;
        std::string_view type;
        std::optional<int> qos;
        std::optional<std::string_view> payload;
    };

    // Autogenerated from https://www.home-assistant.io/integrations/fan.mqtt/.
    struct fan_type {
        std::string_view command_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> qos;
        std::optional<int> speed_range_max;
        std::optional<int> speed_range_min;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_template;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> oscillation_command_template;
        std::optional<std::string_view> oscillation_command_topic;
        std::optional<std::string_view> oscillation_state_topic;
        std::optional<std::string_view> oscillation_value_template;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_off;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> payload_oscillation_off;
        std::optional<std::string_view> payload_oscillation_on;
        std::optional<std::string_view> payload_reset_percentage;
        std::optional<std::string_view> payload_reset_preset_mode;
        std::optional<std::string_view> percentage_command_template;
        std::optional<std::string_view> percentage_command_topic;
        std::optional<std::string_view> percentage_state_topic;
        std::optional<std::string_view> percentage_value_template;
        std::optional<std::string_view> preset_mode_command_template;
        std::optional<std::string_view> preset_mode_command_topic;
        std::optional<std::string_view> preset_mode_state_topic;
        std::optional<std::string_view> preset_mode_value_template;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> state_value_template;
        std::optional<std::string_view> unique_id;
        std::optional<tcb::span<std::string_view>> availability;
        std::optional<tcb::span<std::string_view>> preset_modes;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/humidifier.mqtt/.
    struct humidifier_type {
        std::string_view command_topic;
        std::string_view target_humidity_command_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> max_humidity;
        std::optional<int> min_humidity;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_template;
        std::optional<std::string_view> device_class;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> mode_command_template;
        std::optional<std::string_view> mode_command_topic;
        std::optional<std::string_view> mode_state_template;
        std::optional<std::string_view> mode_state_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_off;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> payload_reset_humidity;
        std::optional<std::string_view> payload_reset_mode;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> state_value_template;
        std::optional<std::string_view> target_humidity_command_template;
        std::optional<std::string_view> target_humidity_state_template;
        std::optional<std::string_view> target_humidity_state_topic;
        std::optional<std::string_view> unique_id;
        std::optional<tcb::span<std::string_view>> availability;
        std::optional<tcb::span<std::string_view>> modes;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/light.mqtt/.
    struct light_type {
        std::string_view command_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> brightness_scale;
        std::optional<int> max_mireds;
        std::optional<int> min_mireds;
        std::optional<int> qos;
        std::optional<int> white_scale;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> brightness_command_topic;
        std::optional<std::string_view> brightness_state_topic;
        std::optional<std::string_view> brightness_value_template;
        std::optional<std::string_view> color_mode_state_topic;
        std::optional<std::string_view> color_mode_value_template;
        std::optional<std::string_view> color_temp_command_template;
        std::optional<std::string_view> color_temp_command_topic;
        std::optional<std::string_view> color_temp_state_topic;
        std::optional<std::string_view> color_temp_value_template;
        std::optional<std::string_view> effect_command_topic;
        std::optional<std::string_view> effect_state_topic;
        std::optional<std::string_view> effect_value_template;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> hs_command_topic;
        std::optional<std::string_view> hs_state_topic;
        std::optional<std::string_view> hs_value_template;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> on_command_type;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_off;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> rgb_command_template;
        std::optional<std::string_view> rgb_command_topic;
        std::optional<std::string_view> rgb_state_topic;
        std::optional<std::string_view> rgb_value_template;
        std::optional<std::string_view> schema;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> state_value_template;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> white_command_topic;
        std::optional<std::string_view> xy_command_topic;
        std::optional<std::string_view> xy_state_topic;
        std::optional<std::string_view> xy_value_template;
        std::optional<tcb::span<std::string_view>> availability;
        std::optional<tcb::span<std::string_view>> effect_list;
    };

    // Autogenerated from https://www.home-assistant.io/integrations/lock.mqtt/.
    struct lock_type {
        std::string_view command_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_lock;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_unlock;
        std::optional<std::string_view> state_locked;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> state_unlocked;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/number.mqtt/.
    struct number_type {
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<double> max;
        std::optional<double> min;
        std::optional<double> step;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_reset;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> unit_of_measurement;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/scene.mqtt/.
    struct scene_type {
        std::optional<bool> enabled_by_default;
        std::optional<bool> retain;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> unique_id;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/select.mqtt/.
    struct select_type {
        std::string_view command_topic;
        tcb::span<std::string_view> options;
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/sensor.mqtt/.
    struct sensor_type {
        std::string_view state_topic;
        std::optional<bool> enabled_by_default;
        std::optional<bool> force_update;
        std::optional<device_type*> device;
        std::optional<int> expire_after;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> device_class;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> last_reset_value_template;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> state_class;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> unit_of_measurement;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/switch.mqtt/.
    struct switch_type {
        std::optional<bool> enabled_by_default;
        std::optional<bool> optimistic;
        std::optional<bool> retain;
        std::optional<device_type*> device;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> command_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_off;
        std::optional<std::string_view> payload_on;
        std::optional<std::string_view> state_off;
        std::optional<std::string_view> state_on;
        std::optional<std::string_view> state_topic;
        std::optional<std::string_view> unique_id;
        std::optional<std::string_view> value_template;
        std::optional<tcb::span<std::string_view>> availability;
    };

    // Autogenerated from
    // https://www.home-assistant.io/integrations/vacuum.mqtt/.
    struct vacuum_type {
        std::optional<bool> enabled_by_default;
        std::optional<bool> retain;
        std::optional<int> qos;
        std::optional<std::string_view> availability_mode;
        std::optional<std::string_view> availability_topic;
        std::optional<std::string_view> battery_level_template;
        std::optional<std::string_view> battery_level_topic;
        std::optional<std::string_view> charging_template;
        std::optional<std::string_view> charging_topic;
        std::optional<std::string_view> cleaning_template;
        std::optional<std::string_view> cleaning_topic;
        std::optional<std::string_view> command_topic;
        std::optional<std::string_view> docked_template;
        std::optional<std::string_view> docked_topic;
        std::optional<std::string_view> entity_category;
        std::optional<std::string_view> error_template;
        std::optional<std::string_view> error_topic;
        std::optional<std::string_view> fan_speed_template;
        std::optional<std::string_view> fan_speed_topic;
        std::optional<std::string_view> icon;
        std::optional<std::string_view> json_attributes_template;
        std::optional<std::string_view> json_attributes_topic;
        std::optional<std::string_view> name;
        std::optional<std::string_view> payload_available;
        std::optional<std::string_view> payload_clean_spot;
        std::optional<std::string_view> payload_locate;
        std::optional<std::string_view> payload_not_available;
        std::optional<std::string_view> payload_return_to_base;
        std::optional<std::string_view> payload_start_pause;
        std::optional<std::string_view> payload_stop;
        std::optional<std::string_view> payload_turn_off;
        std::optional<std::string_view> payload_turn_on;
        std::optional<std::string_view> schema;
        std::optional<std::string_view> send_command_topic;
        std::optional<std::string_view> set_fan_speed_topic;
        std::optional<std::string_view> unique_id;
        std::optional<tcb::span<std::string_view>> availability;
        std::optional<tcb::span<std::string_view>> fan_speed_list;
        std::optional<tcb::span<std::string_view>> supported_features;
    };

    rapidjson::Document serialize(
        const alarm_control_panel_type& device) noexcept;

    rapidjson::Document serialize(const binary_sensor_type& device) noexcept;

    rapidjson::Document serialize(const camera_type& device) noexcept;

    rapidjson::Document serialize(const cover_type& device) noexcept;

    rapidjson::Document serialize(const device_tracker_type& device) noexcept;

    rapidjson::Document serialize(const device_trigger_type& device) noexcept;

    rapidjson::Document serialize(const fan_type& device) noexcept;

    rapidjson::Document serialize(const humidifier_type& device) noexcept;

    rapidjson::Document serialize(const light_type& device) noexcept;

    rapidjson::Document serialize(const lock_type& device) noexcept;

    rapidjson::Document serialize(const number_type& device) noexcept;

    rapidjson::Document serialize(const scene_type& device) noexcept;

    rapidjson::Document serialize(const select_type& device) noexcept;

    rapidjson::Document serialize(const sensor_type& device) noexcept;

    rapidjson::Document serialize(const switch_type& device) noexcept;

    rapidjson::Document serialize(const vacuum_type& device) noexcept;
} // namespace b2h::hass
#endif
