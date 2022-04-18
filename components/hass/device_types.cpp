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

#include "hass/device_types.hpp"

namespace b2h::hass
{
    namespace
    {
        inline void serialize_device_type(
            const device_type& device, rapidjson::Document& doc)
        {
            using namespace rapidjson;

            Value v(kObjectType);
            if (device.manufacturer.has_value())
            {
                v.AddMember("mf",
                    Value().SetString(
                        StringRef(device.manufacturer.value().data(),
                            device.manufacturer.value().size())),
                    doc.GetAllocator());
            }
            if (device.model.has_value())
            {
                v.AddMember("mdl",
                    Value().SetString(StringRef(device.model.value().data(),
                        device.model.value().size())),
                    doc.GetAllocator());
            }
            if (device.name.has_value())
            {
                v.AddMember("name",
                    Value().SetString(StringRef(device.name.value().data(),
                        device.name.value().size())),
                    doc.GetAllocator());
            }
            if (device.suggested_area.has_value())
            {
                v.AddMember("sa",
                    Value().SetString(
                        StringRef(device.suggested_area.value().data(),
                            device.suggested_area.value().size())),
                    doc.GetAllocator());
            }
            if (device.sw_version.has_value())
            {
                v.AddMember("sw",
                    Value().SetString(
                        StringRef(device.sw_version.value().data(),
                            device.sw_version.value().size())),
                    doc.GetAllocator());
            }
            if (device.via_device.has_value())
            {
                v.AddMember("mf",
                    Value().SetString(
                        StringRef(device.via_device.value().data(),
                            device.via_device.value().size())),
                    doc.GetAllocator());
            }
            if (device.connections.has_value())
            {
                Value a;
                a.SetArray();
                for (const auto& [key, value] : device.connections.value())
                {
                    Value tuple;
                    tuple.SetArray();

                    tuple.PushBack(
                        Value().SetString(StringRef(key.data(), key.size())),
                        doc.GetAllocator());
                    tuple.PushBack(Value().SetString(
                                       StringRef(value.data(), value.size())),
                        doc.GetAllocator());

                    a.PushBack(tuple, doc.GetAllocator());
                }
                v.AddMember("cns", std::move(a), doc.GetAllocator());
            }
            if (device.identifiers.has_value())
            {
                Value a;
                a.SetArray();
                for (const auto& str : device.identifiers.value())
                {
                    a.PushBack(
                        Value().SetString(StringRef(str.data(), str.size())),
                        doc.GetAllocator());
                }
                v.AddMember("ids", std::move(a), doc.GetAllocator());
            }
            doc.AddMember("dev", std::move(v), doc.GetAllocator());
        }
    } // namespace

    rapidjson::Document serialize(
        const alarm_control_panel_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        d.AddMember("stat_t",
            Value().SetString(StringRef(device.state_topic.data(),
                device.state_topic.size())),
            d.GetAllocator());
        if (device.code_arm_required.has_value())
        {
            d.AddMember("cod_arm_req",
                Value().SetBool(device.code_arm_required.value()),
                d.GetAllocator());
        }
        if (device.code_disarm_required.has_value())
        {
            d.AddMember("cod_dis_req",
                Value().SetBool(device.code_disarm_required.value()),
                d.GetAllocator());
        }
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.code.has_value())
        {
            d.AddMember("code",
                Value().SetString(StringRef(device.code.value().data(),
                    device.code.value().size())),
                d.GetAllocator());
        }
        if (device.command_template.has_value())
        {
            d.AddMember("cmd_tpl",
                Value().SetString(
                    StringRef(device.command_template.value().data(),
                        device.command_template.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_arm_away.has_value())
        {
            d.AddMember("pl_arm_away",
                Value().SetString(
                    StringRef(device.payload_arm_away.value().data(),
                        device.payload_arm_away.value().size())),
                d.GetAllocator());
        }
        if (device.payload_arm_custom_bypass.has_value())
        {
            d.AddMember("pl_arm_custom_b",
                Value().SetString(
                    StringRef(device.payload_arm_custom_bypass.value().data(),
                        device.payload_arm_custom_bypass.value().size())),
                d.GetAllocator());
        }
        if (device.payload_arm_home.has_value())
        {
            d.AddMember("pl_arm_home",
                Value().SetString(
                    StringRef(device.payload_arm_home.value().data(),
                        device.payload_arm_home.value().size())),
                d.GetAllocator());
        }
        if (device.payload_arm_night.has_value())
        {
            d.AddMember("pl_arm_nite",
                Value().SetString(
                    StringRef(device.payload_arm_night.value().data(),
                        device.payload_arm_night.value().size())),
                d.GetAllocator());
        }
        if (device.payload_arm_vacation.has_value())
        {
            d.AddMember("payload_arm_vacation",
                Value().SetString(
                    StringRef(device.payload_arm_vacation.value().data(),
                        device.payload_arm_vacation.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_disarm.has_value())
        {
            d.AddMember("pl_disarm",
                Value().SetString(
                    StringRef(device.payload_disarm.value().data(),
                        device.payload_disarm.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const binary_sensor_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("stat_t",
            Value().SetString(StringRef(device.state_topic.data(),
                device.state_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.force_update.has_value())
        {
            d.AddMember("frc_upd",
                Value().SetBool(device.force_update.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.expire_after.has_value())
        {
            d.AddMember("exp_aft",
                Value().SetInt(device.expire_after.value()),
                d.GetAllocator());
        }
        if (device.off_delay.has_value())
        {
            d.AddMember("off_dly",
                Value().SetInt(device.off_delay.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.device_class.has_value())
        {
            d.AddMember("dev_cla",
                Value().SetString(StringRef(device.device_class.value().data(),
                    device.device_class.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_off.has_value())
        {
            d.AddMember("pl_off",
                Value().SetString(StringRef(device.payload_off.value().data(),
                    device.payload_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const camera_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("t",
            Value().SetString(
                StringRef(device.topic.data(), device.topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const cover_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.tilt_optimistic.has_value())
        {
            d.AddMember("tilt_opt",
                Value().SetBool(device.tilt_optimistic.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.position_closed.has_value())
        {
            d.AddMember("pos_clsd",
                Value().SetInt(device.position_closed.value()),
                d.GetAllocator());
        }
        if (device.position_open.has_value())
        {
            d.AddMember("pos_open",
                Value().SetInt(device.position_open.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.tilt_closed_value.has_value())
        {
            d.AddMember("tilt_clsd_val",
                Value().SetInt(device.tilt_closed_value.value()),
                d.GetAllocator());
        }
        if (device.tilt_max.has_value())
        {
            d.AddMember("tilt_max",
                Value().SetInt(device.tilt_max.value()),
                d.GetAllocator());
        }
        if (device.tilt_min.has_value())
        {
            d.AddMember("tilt_min",
                Value().SetInt(device.tilt_min.value()),
                d.GetAllocator());
        }
        if (device.tilt_opened_value.has_value())
        {
            d.AddMember("tilt_opnd_val",
                Value().SetInt(device.tilt_opened_value.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_topic.has_value())
        {
            d.AddMember("cmd_t",
                Value().SetString(StringRef(device.command_topic.value().data(),
                    device.command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.device_class.has_value())
        {
            d.AddMember("dev_cla",
                Value().SetString(StringRef(device.device_class.value().data(),
                    device.device_class.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_close.has_value())
        {
            d.AddMember("pl_cls",
                Value().SetString(StringRef(device.payload_close.value().data(),
                    device.payload_close.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_open.has_value())
        {
            d.AddMember("pl_open",
                Value().SetString(StringRef(device.payload_open.value().data(),
                    device.payload_open.value().size())),
                d.GetAllocator());
        }
        if (device.payload_stop.has_value())
        {
            d.AddMember("pl_stop",
                Value().SetString(StringRef(device.payload_stop.value().data(),
                    device.payload_stop.value().size())),
                d.GetAllocator());
        }
        if (device.position_template.has_value())
        {
            d.AddMember("pos_tpl",
                Value().SetString(
                    StringRef(device.position_template.value().data(),
                        device.position_template.value().size())),
                d.GetAllocator());
        }
        if (device.position_topic.has_value())
        {
            d.AddMember("pos_t",
                Value().SetString(
                    StringRef(device.position_topic.value().data(),
                        device.position_topic.value().size())),
                d.GetAllocator());
        }
        if (device.set_position_template.has_value())
        {
            d.AddMember("set_pos_tpl",
                Value().SetString(
                    StringRef(device.set_position_template.value().data(),
                        device.set_position_template.value().size())),
                d.GetAllocator());
        }
        if (device.set_position_topic.has_value())
        {
            d.AddMember("set_pos_t",
                Value().SetString(
                    StringRef(device.set_position_topic.value().data(),
                        device.set_position_topic.value().size())),
                d.GetAllocator());
        }
        if (device.state_closed.has_value())
        {
            d.AddMember("stat_clsd",
                Value().SetString(StringRef(device.state_closed.value().data(),
                    device.state_closed.value().size())),
                d.GetAllocator());
        }
        if (device.state_closing.has_value())
        {
            d.AddMember("stat_closing",
                Value().SetString(StringRef(device.state_closing.value().data(),
                    device.state_closing.value().size())),
                d.GetAllocator());
        }
        if (device.state_open.has_value())
        {
            d.AddMember("stat_open",
                Value().SetString(StringRef(device.state_open.value().data(),
                    device.state_open.value().size())),
                d.GetAllocator());
        }
        if (device.state_opening.has_value())
        {
            d.AddMember("stat_opening",
                Value().SetString(StringRef(device.state_opening.value().data(),
                    device.state_opening.value().size())),
                d.GetAllocator());
        }
        if (device.state_stopped.has_value())
        {
            d.AddMember("stat_stopped",
                Value().SetString(StringRef(device.state_stopped.value().data(),
                    device.state_stopped.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.tilt_command_template.has_value())
        {
            d.AddMember("tilt_cmd_tpl",
                Value().SetString(
                    StringRef(device.tilt_command_template.value().data(),
                        device.tilt_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.tilt_command_topic.has_value())
        {
            d.AddMember("tilt_cmd_t",
                Value().SetString(
                    StringRef(device.tilt_command_topic.value().data(),
                        device.tilt_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.tilt_status_template.has_value())
        {
            d.AddMember("tilt_status_tpl",
                Value().SetString(
                    StringRef(device.tilt_status_template.value().data(),
                        device.tilt_status_template.value().size())),
                d.GetAllocator());
        }
        if (device.tilt_status_topic.has_value())
        {
            d.AddMember("tilt_status_t",
                Value().SetString(
                    StringRef(device.tilt_status_topic.value().data(),
                        device.tilt_status_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const device_tracker_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        Value a;
        a.SetArray();
        for (const auto& str : device.devices)
        {
            a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                d.GetAllocator());
        }
        d.AddMember("devices", std::move(a), d.GetAllocator());
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.payload_home.has_value())
        {
            d.AddMember("pl_home",
                Value().SetString(StringRef(device.payload_home.value().data(),
                    device.payload_home.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_home.has_value())
        {
            d.AddMember("pl_not_home",
                Value().SetString(
                    StringRef(device.payload_not_home.value().data(),
                        device.payload_not_home.value().size())),
                d.GetAllocator());
        }
        if (device.source_type.has_value())
        {
            d.AddMember("src_type",
                Value().SetString(StringRef(device.source_type.value().data(),
                    device.source_type.value().size())),
                d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const device_trigger_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();

        serialize_device_type(device.device, d);

        d.AddMember("atype",
            Value().SetString(StringRef(device.automation_type.data(),
                device.automation_type.size())),
            d.GetAllocator());
        d.AddMember("stype",
            Value().SetString(
                StringRef(device.subtype.data(), device.subtype.size())),
            d.GetAllocator());
        d.AddMember("t",
            Value().SetString(
                StringRef(device.topic.data(), device.topic.size())),
            d.GetAllocator());
        d.AddMember("type",
            Value().SetString(
                StringRef(device.type.data(), device.type.size())),
            d.GetAllocator());
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.payload.has_value())
        {
            d.AddMember("pl",
                Value().SetString(StringRef(device.payload.value().data(),
                    device.payload.value().size())),
                d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const fan_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.speed_range_max.has_value())
        {
            d.AddMember("spd_rng_max",
                Value().SetInt(device.speed_range_max.value()),
                d.GetAllocator());
        }
        if (device.speed_range_min.has_value())
        {
            d.AddMember("spd_rng_min",
                Value().SetInt(device.speed_range_min.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_template.has_value())
        {
            d.AddMember("cmd_tpl",
                Value().SetString(
                    StringRef(device.command_template.value().data(),
                        device.command_template.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.oscillation_command_template.has_value())
        {
            d.AddMember("osc_cmd_tpl",
                Value().SetString(StringRef(
                    device.oscillation_command_template.value().data(),
                    device.oscillation_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.oscillation_command_topic.has_value())
        {
            d.AddMember("osc_cmd_t",
                Value().SetString(
                    StringRef(device.oscillation_command_topic.value().data(),
                        device.oscillation_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.oscillation_state_topic.has_value())
        {
            d.AddMember("osc_stat_t",
                Value().SetString(
                    StringRef(device.oscillation_state_topic.value().data(),
                        device.oscillation_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.oscillation_value_template.has_value())
        {
            d.AddMember("osc_val_tpl",
                Value().SetString(
                    StringRef(device.oscillation_value_template.value().data(),
                        device.oscillation_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_off.has_value())
        {
            d.AddMember("pl_off",
                Value().SetString(StringRef(device.payload_off.value().data(),
                    device.payload_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.payload_oscillation_off.has_value())
        {
            d.AddMember("pl_osc_off",
                Value().SetString(
                    StringRef(device.payload_oscillation_off.value().data(),
                        device.payload_oscillation_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_oscillation_on.has_value())
        {
            d.AddMember("pl_osc_on",
                Value().SetString(
                    StringRef(device.payload_oscillation_on.value().data(),
                        device.payload_oscillation_on.value().size())),
                d.GetAllocator());
        }
        if (device.payload_reset_percentage.has_value())
        {
            d.AddMember("pl_rst_pct",
                Value().SetString(
                    StringRef(device.payload_reset_percentage.value().data(),
                        device.payload_reset_percentage.value().size())),
                d.GetAllocator());
        }
        if (device.payload_reset_preset_mode.has_value())
        {
            d.AddMember("pl_rst_pr_mode",
                Value().SetString(
                    StringRef(device.payload_reset_preset_mode.value().data(),
                        device.payload_reset_preset_mode.value().size())),
                d.GetAllocator());
        }
        if (device.percentage_command_template.has_value())
        {
            d.AddMember("pct_cmd_tpl",
                Value().SetString(
                    StringRef(device.percentage_command_template.value().data(),
                        device.percentage_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.percentage_command_topic.has_value())
        {
            d.AddMember("pct_cmd_t",
                Value().SetString(
                    StringRef(device.percentage_command_topic.value().data(),
                        device.percentage_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.percentage_state_topic.has_value())
        {
            d.AddMember("pct_stat_t",
                Value().SetString(
                    StringRef(device.percentage_state_topic.value().data(),
                        device.percentage_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.percentage_value_template.has_value())
        {
            d.AddMember("pct_val_tpl",
                Value().SetString(
                    StringRef(device.percentage_value_template.value().data(),
                        device.percentage_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.preset_mode_command_template.has_value())
        {
            d.AddMember("pr_mode_cmd_tpl",
                Value().SetString(StringRef(
                    device.preset_mode_command_template.value().data(),
                    device.preset_mode_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.preset_mode_command_topic.has_value())
        {
            d.AddMember("pr_mode_cmd_t",
                Value().SetString(
                    StringRef(device.preset_mode_command_topic.value().data(),
                        device.preset_mode_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.preset_mode_state_topic.has_value())
        {
            d.AddMember("pr_mode_stat_t",
                Value().SetString(
                    StringRef(device.preset_mode_state_topic.value().data(),
                        device.preset_mode_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.preset_mode_value_template.has_value())
        {
            d.AddMember("pr_mode_val_tpl",
                Value().SetString(
                    StringRef(device.preset_mode_value_template.value().data(),
                        device.preset_mode_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.state_value_template.has_value())
        {
            d.AddMember("stat_val_tpl",
                Value().SetString(
                    StringRef(device.state_value_template.value().data(),
                        device.state_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        if (device.preset_modes.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.preset_modes.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("pr_modes", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const humidifier_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        d.AddMember("hum_cmd_t",
            Value().SetString(
                StringRef(device.target_humidity_command_topic.data(),
                    device.target_humidity_command_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.max_humidity.has_value())
        {
            d.AddMember("max_hum",
                Value().SetInt(device.max_humidity.value()),
                d.GetAllocator());
        }
        if (device.min_humidity.has_value())
        {
            d.AddMember("min_hum",
                Value().SetInt(device.min_humidity.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_template.has_value())
        {
            d.AddMember("cmd_tpl",
                Value().SetString(
                    StringRef(device.command_template.value().data(),
                        device.command_template.value().size())),
                d.GetAllocator());
        }
        if (device.device_class.has_value())
        {
            d.AddMember("dev_cla",
                Value().SetString(StringRef(device.device_class.value().data(),
                    device.device_class.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.mode_command_template.has_value())
        {
            d.AddMember("mode_cmd_tpl",
                Value().SetString(
                    StringRef(device.mode_command_template.value().data(),
                        device.mode_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.mode_command_topic.has_value())
        {
            d.AddMember("mode_cmd_t",
                Value().SetString(
                    StringRef(device.mode_command_topic.value().data(),
                        device.mode_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.mode_state_template.has_value())
        {
            d.AddMember("mode_stat_tpl",
                Value().SetString(
                    StringRef(device.mode_state_template.value().data(),
                        device.mode_state_template.value().size())),
                d.GetAllocator());
        }
        if (device.mode_state_topic.has_value())
        {
            d.AddMember("mode_stat_t",
                Value().SetString(
                    StringRef(device.mode_state_topic.value().data(),
                        device.mode_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_off.has_value())
        {
            d.AddMember("pl_off",
                Value().SetString(StringRef(device.payload_off.value().data(),
                    device.payload_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.payload_reset_humidity.has_value())
        {
            d.AddMember("pl_rst_hum",
                Value().SetString(
                    StringRef(device.payload_reset_humidity.value().data(),
                        device.payload_reset_humidity.value().size())),
                d.GetAllocator());
        }
        if (device.payload_reset_mode.has_value())
        {
            d.AddMember("pl_rst_mode",
                Value().SetString(
                    StringRef(device.payload_reset_mode.value().data(),
                        device.payload_reset_mode.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.state_value_template.has_value())
        {
            d.AddMember("stat_val_tpl",
                Value().SetString(
                    StringRef(device.state_value_template.value().data(),
                        device.state_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.target_humidity_command_template.has_value())
        {
            d.AddMember("hum_cmd_tpl",
                Value().SetString(StringRef(
                    device.target_humidity_command_template.value().data(),
                    device.target_humidity_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.target_humidity_state_template.has_value())
        {
            d.AddMember("hum_stat_tpl",
                Value().SetString(StringRef(
                    device.target_humidity_state_template.value().data(),
                    device.target_humidity_state_template.value().size())),
                d.GetAllocator());
        }
        if (device.target_humidity_state_topic.has_value())
        {
            d.AddMember("hum_stat_t",
                Value().SetString(
                    StringRef(device.target_humidity_state_topic.value().data(),
                        device.target_humidity_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        if (device.modes.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.modes.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("modes", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const light_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.brightness_scale.has_value())
        {
            d.AddMember("bri_scl",
                Value().SetInt(device.brightness_scale.value()),
                d.GetAllocator());
        }
        if (device.max_mireds.has_value())
        {
            d.AddMember("max_mirs",
                Value().SetInt(device.max_mireds.value()),
                d.GetAllocator());
        }
        if (device.min_mireds.has_value())
        {
            d.AddMember("min_mirs",
                Value().SetInt(device.min_mireds.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.white_scale.has_value())
        {
            d.AddMember("white_scale",
                Value().SetInt(device.white_scale.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.brightness_command_topic.has_value())
        {
            d.AddMember("bri_cmd_t",
                Value().SetString(
                    StringRef(device.brightness_command_topic.value().data(),
                        device.brightness_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.brightness_state_topic.has_value())
        {
            d.AddMember("bri_stat_t",
                Value().SetString(
                    StringRef(device.brightness_state_topic.value().data(),
                        device.brightness_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.brightness_value_template.has_value())
        {
            d.AddMember("bri_val_tpl",
                Value().SetString(
                    StringRef(device.brightness_value_template.value().data(),
                        device.brightness_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.color_mode_state_topic.has_value())
        {
            d.AddMember("color_mode_state_topic",
                Value().SetString(
                    StringRef(device.color_mode_state_topic.value().data(),
                        device.color_mode_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.color_mode_value_template.has_value())
        {
            d.AddMember("color_mode_value_template",
                Value().SetString(
                    StringRef(device.color_mode_value_template.value().data(),
                        device.color_mode_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.color_temp_command_template.has_value())
        {
            d.AddMember("clr_temp_cmd_tpl",
                Value().SetString(
                    StringRef(device.color_temp_command_template.value().data(),
                        device.color_temp_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.color_temp_command_topic.has_value())
        {
            d.AddMember("clr_temp_cmd_t",
                Value().SetString(
                    StringRef(device.color_temp_command_topic.value().data(),
                        device.color_temp_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.color_temp_state_topic.has_value())
        {
            d.AddMember("clr_temp_stat_t",
                Value().SetString(
                    StringRef(device.color_temp_state_topic.value().data(),
                        device.color_temp_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.color_temp_value_template.has_value())
        {
            d.AddMember("clr_temp_val_tpl",
                Value().SetString(
                    StringRef(device.color_temp_value_template.value().data(),
                        device.color_temp_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.effect_command_topic.has_value())
        {
            d.AddMember("fx_cmd_t",
                Value().SetString(
                    StringRef(device.effect_command_topic.value().data(),
                        device.effect_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.effect_state_topic.has_value())
        {
            d.AddMember("fx_stat_t",
                Value().SetString(
                    StringRef(device.effect_state_topic.value().data(),
                        device.effect_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.effect_value_template.has_value())
        {
            d.AddMember("fx_val_tpl",
                Value().SetString(
                    StringRef(device.effect_value_template.value().data(),
                        device.effect_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.hs_command_topic.has_value())
        {
            d.AddMember("hs_cmd_t",
                Value().SetString(
                    StringRef(device.hs_command_topic.value().data(),
                        device.hs_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.hs_state_topic.has_value())
        {
            d.AddMember("hs_stat_t",
                Value().SetString(
                    StringRef(device.hs_state_topic.value().data(),
                        device.hs_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.hs_value_template.has_value())
        {
            d.AddMember("hs_val_tpl",
                Value().SetString(
                    StringRef(device.hs_value_template.value().data(),
                        device.hs_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.on_command_type.has_value())
        {
            d.AddMember("on_cmd_type",
                Value().SetString(
                    StringRef(device.on_command_type.value().data(),
                        device.on_command_type.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_off.has_value())
        {
            d.AddMember("pl_off",
                Value().SetString(StringRef(device.payload_off.value().data(),
                    device.payload_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.rgb_command_template.has_value())
        {
            d.AddMember("rgb_cmd_tpl",
                Value().SetString(
                    StringRef(device.rgb_command_template.value().data(),
                        device.rgb_command_template.value().size())),
                d.GetAllocator());
        }
        if (device.rgb_command_topic.has_value())
        {
            d.AddMember("rgb_cmd_t",
                Value().SetString(
                    StringRef(device.rgb_command_topic.value().data(),
                        device.rgb_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.rgb_state_topic.has_value())
        {
            d.AddMember("rgb_stat_t",
                Value().SetString(
                    StringRef(device.rgb_state_topic.value().data(),
                        device.rgb_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.rgb_value_template.has_value())
        {
            d.AddMember("rgb_val_tpl",
                Value().SetString(
                    StringRef(device.rgb_value_template.value().data(),
                        device.rgb_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.schema.has_value())
        {
            d.AddMember("schema",
                Value().SetString(StringRef(device.schema.value().data(),
                    device.schema.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.state_value_template.has_value())
        {
            d.AddMember("stat_val_tpl",
                Value().SetString(
                    StringRef(device.state_value_template.value().data(),
                        device.state_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.white_command_topic.has_value())
        {
            d.AddMember("white_command_topic",
                Value().SetString(
                    StringRef(device.white_command_topic.value().data(),
                        device.white_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.xy_command_topic.has_value())
        {
            d.AddMember("xy_cmd_t",
                Value().SetString(
                    StringRef(device.xy_command_topic.value().data(),
                        device.xy_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.xy_state_topic.has_value())
        {
            d.AddMember("xy_stat_t",
                Value().SetString(
                    StringRef(device.xy_state_topic.value().data(),
                        device.xy_state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.xy_value_template.has_value())
        {
            d.AddMember("xy_val_tpl",
                Value().SetString(
                    StringRef(device.xy_value_template.value().data(),
                        device.xy_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        if (device.effect_list.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.effect_list.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("fx_list", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const lock_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_lock.has_value())
        {
            d.AddMember("pl_lock",
                Value().SetString(StringRef(device.payload_lock.value().data(),
                    device.payload_lock.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_unlock.has_value())
        {
            d.AddMember("pl_unlk",
                Value().SetString(
                    StringRef(device.payload_unlock.value().data(),
                        device.payload_unlock.value().size())),
                d.GetAllocator());
        }
        if (device.state_locked.has_value())
        {
            d.AddMember("stat_locked",
                Value().SetString(StringRef(device.state_locked.value().data(),
                    device.state_locked.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.state_unlocked.has_value())
        {
            d.AddMember("stat_unlocked",
                Value().SetString(
                    StringRef(device.state_unlocked.value().data(),
                        device.state_unlocked.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const number_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.max.has_value())
        {
            d.AddMember("max",
                Value().SetDouble(device.max.value()),
                d.GetAllocator());
        }
        if (device.min.has_value())
        {
            d.AddMember("min",
                Value().SetDouble(device.min.value()),
                d.GetAllocator());
        }
        if (device.step.has_value())
        {
            d.AddMember("step",
                Value().SetDouble(device.step.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_topic.has_value())
        {
            d.AddMember("cmd_t",
                Value().SetString(StringRef(device.command_topic.value().data(),
                    device.command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_reset.has_value())
        {
            d.AddMember("payload_reset",
                Value().SetString(StringRef(device.payload_reset.value().data(),
                    device.payload_reset.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.unit_of_measurement.has_value())
        {
            d.AddMember("unit_of_meas",
                Value().SetString(
                    StringRef(device.unit_of_measurement.value().data(),
                        device.unit_of_measurement.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const scene_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_topic.has_value())
        {
            d.AddMember("cmd_t",
                Value().SetString(StringRef(device.command_topic.value().data(),
                    device.command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const select_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("cmd_t",
            Value().SetString(StringRef(device.command_topic.data(),
                device.command_topic.size())),
            d.GetAllocator());
        Value a;
        a.SetArray();
        for (const auto& str : device.options)
        {
            a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                d.GetAllocator());
        }
        d.AddMember("options", std::move(a), d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const sensor_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        d.AddMember("stat_t",
            Value().SetString(StringRef(device.state_topic.data(),
                device.state_topic.size())),
            d.GetAllocator());
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.force_update.has_value())
        {
            d.AddMember("frc_upd",
                Value().SetBool(device.force_update.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.expire_after.has_value())
        {
            d.AddMember("exp_aft",
                Value().SetInt(device.expire_after.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.device_class.has_value())
        {
            d.AddMember("dev_cla",
                Value().SetString(StringRef(device.device_class.value().data(),
                    device.device_class.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.last_reset_value_template.has_value())
        {
            d.AddMember("last_reset_value_template",
                Value().SetString(
                    StringRef(device.last_reset_value_template.value().data(),
                        device.last_reset_value_template.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.state_class.has_value())
        {
            d.AddMember("stat_cla",
                Value().SetString(StringRef(device.state_class.value().data(),
                    device.state_class.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.unit_of_measurement.has_value())
        {
            d.AddMember("unit_of_meas",
                Value().SetString(
                    StringRef(device.unit_of_measurement.value().data(),
                        device.unit_of_measurement.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const switch_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.optimistic.has_value())
        {
            d.AddMember("opt",
                Value().SetBool(device.optimistic.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.device.has_value())
        {
            serialize_device_type(device.device.value(), d);
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_topic.has_value())
        {
            d.AddMember("cmd_t",
                Value().SetString(StringRef(device.command_topic.value().data(),
                    device.command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_off.has_value())
        {
            d.AddMember("pl_off",
                Value().SetString(StringRef(device.payload_off.value().data(),
                    device.payload_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_on.has_value())
        {
            d.AddMember("pl_on",
                Value().SetString(StringRef(device.payload_on.value().data(),
                    device.payload_on.value().size())),
                d.GetAllocator());
        }
        if (device.state_off.has_value())
        {
            d.AddMember("stat_off",
                Value().SetString(StringRef(device.state_off.value().data(),
                    device.state_off.value().size())),
                d.GetAllocator());
        }
        if (device.state_on.has_value())
        {
            d.AddMember("stat_on",
                Value().SetString(StringRef(device.state_on.value().data(),
                    device.state_on.value().size())),
                d.GetAllocator());
        }
        if (device.state_topic.has_value())
        {
            d.AddMember("stat_t",
                Value().SetString(StringRef(device.state_topic.value().data(),
                    device.state_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.value_template.has_value())
        {
            d.AddMember("val_tpl",
                Value().SetString(
                    StringRef(device.value_template.value().data(),
                        device.value_template.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        return d;
    }

    rapidjson::Document serialize(const vacuum_type& device) noexcept
    {
        using namespace rapidjson;
        Document d;
        d.SetObject();
        if (device.enabled_by_default.has_value())
        {
            d.AddMember("enabled_by_default",
                Value().SetBool(device.enabled_by_default.value()),
                d.GetAllocator());
        }
        if (device.retain.has_value())
        {
            d.AddMember("ret",
                Value().SetBool(device.retain.value()),
                d.GetAllocator());
        }
        if (device.qos.has_value())
        {
            d.AddMember("qos",
                Value().SetInt(device.qos.value()),
                d.GetAllocator());
        }
        if (device.availability_mode.has_value())
        {
            d.AddMember("avty_mode",
                Value().SetString(
                    StringRef(device.availability_mode.value().data(),
                        device.availability_mode.value().size())),
                d.GetAllocator());
        }
        if (device.availability_topic.has_value())
        {
            d.AddMember("avty_t",
                Value().SetString(
                    StringRef(device.availability_topic.value().data(),
                        device.availability_topic.value().size())),
                d.GetAllocator());
        }
        if (device.battery_level_template.has_value())
        {
            d.AddMember("bat_lev_tpl",
                Value().SetString(
                    StringRef(device.battery_level_template.value().data(),
                        device.battery_level_template.value().size())),
                d.GetAllocator());
        }
        if (device.battery_level_topic.has_value())
        {
            d.AddMember("bat_lev_t",
                Value().SetString(
                    StringRef(device.battery_level_topic.value().data(),
                        device.battery_level_topic.value().size())),
                d.GetAllocator());
        }
        if (device.charging_template.has_value())
        {
            d.AddMember("chrg_tpl",
                Value().SetString(
                    StringRef(device.charging_template.value().data(),
                        device.charging_template.value().size())),
                d.GetAllocator());
        }
        if (device.charging_topic.has_value())
        {
            d.AddMember("chrg_t",
                Value().SetString(
                    StringRef(device.charging_topic.value().data(),
                        device.charging_topic.value().size())),
                d.GetAllocator());
        }
        if (device.cleaning_template.has_value())
        {
            d.AddMember("cln_tpl",
                Value().SetString(
                    StringRef(device.cleaning_template.value().data(),
                        device.cleaning_template.value().size())),
                d.GetAllocator());
        }
        if (device.cleaning_topic.has_value())
        {
            d.AddMember("cln_t",
                Value().SetString(
                    StringRef(device.cleaning_topic.value().data(),
                        device.cleaning_topic.value().size())),
                d.GetAllocator());
        }
        if (device.command_topic.has_value())
        {
            d.AddMember("cmd_t",
                Value().SetString(StringRef(device.command_topic.value().data(),
                    device.command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.docked_template.has_value())
        {
            d.AddMember("dock_tpl",
                Value().SetString(
                    StringRef(device.docked_template.value().data(),
                        device.docked_template.value().size())),
                d.GetAllocator());
        }
        if (device.docked_topic.has_value())
        {
            d.AddMember("dock_t",
                Value().SetString(StringRef(device.docked_topic.value().data(),
                    device.docked_topic.value().size())),
                d.GetAllocator());
        }
        if (device.entity_category.has_value())
        {
            d.AddMember("entity_category",
                Value().SetString(
                    StringRef(device.entity_category.value().data(),
                        device.entity_category.value().size())),
                d.GetAllocator());
        }
        if (device.error_template.has_value())
        {
            d.AddMember("err_tpl",
                Value().SetString(
                    StringRef(device.error_template.value().data(),
                        device.error_template.value().size())),
                d.GetAllocator());
        }
        if (device.error_topic.has_value())
        {
            d.AddMember("err_t",
                Value().SetString(StringRef(device.error_topic.value().data(),
                    device.error_topic.value().size())),
                d.GetAllocator());
        }
        if (device.fan_speed_template.has_value())
        {
            d.AddMember("fanspd_tpl",
                Value().SetString(
                    StringRef(device.fan_speed_template.value().data(),
                        device.fan_speed_template.value().size())),
                d.GetAllocator());
        }
        if (device.fan_speed_topic.has_value())
        {
            d.AddMember("fanspd_t",
                Value().SetString(
                    StringRef(device.fan_speed_topic.value().data(),
                        device.fan_speed_topic.value().size())),
                d.GetAllocator());
        }
        if (device.icon.has_value())
        {
            d.AddMember("ic",
                Value().SetString(StringRef(device.icon.value().data(),
                    device.icon.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_template.has_value())
        {
            d.AddMember("json_attr_tpl",
                Value().SetString(
                    StringRef(device.json_attributes_template.value().data(),
                        device.json_attributes_template.value().size())),
                d.GetAllocator());
        }
        if (device.json_attributes_topic.has_value())
        {
            d.AddMember("json_attr_t",
                Value().SetString(
                    StringRef(device.json_attributes_topic.value().data(),
                        device.json_attributes_topic.value().size())),
                d.GetAllocator());
        }
        if (device.name.has_value())
        {
            d.AddMember("name",
                Value().SetString(StringRef(device.name.value().data(),
                    device.name.value().size())),
                d.GetAllocator());
        }
        if (device.payload_available.has_value())
        {
            d.AddMember("pl_avail",
                Value().SetString(
                    StringRef(device.payload_available.value().data(),
                        device.payload_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_clean_spot.has_value())
        {
            d.AddMember("pl_cln_sp",
                Value().SetString(
                    StringRef(device.payload_clean_spot.value().data(),
                        device.payload_clean_spot.value().size())),
                d.GetAllocator());
        }
        if (device.payload_locate.has_value())
        {
            d.AddMember("pl_loc",
                Value().SetString(
                    StringRef(device.payload_locate.value().data(),
                        device.payload_locate.value().size())),
                d.GetAllocator());
        }
        if (device.payload_not_available.has_value())
        {
            d.AddMember("pl_not_avail",
                Value().SetString(
                    StringRef(device.payload_not_available.value().data(),
                        device.payload_not_available.value().size())),
                d.GetAllocator());
        }
        if (device.payload_return_to_base.has_value())
        {
            d.AddMember("pl_ret",
                Value().SetString(
                    StringRef(device.payload_return_to_base.value().data(),
                        device.payload_return_to_base.value().size())),
                d.GetAllocator());
        }
        if (device.payload_start_pause.has_value())
        {
            d.AddMember("pl_stpa",
                Value().SetString(
                    StringRef(device.payload_start_pause.value().data(),
                        device.payload_start_pause.value().size())),
                d.GetAllocator());
        }
        if (device.payload_stop.has_value())
        {
            d.AddMember("pl_stop",
                Value().SetString(StringRef(device.payload_stop.value().data(),
                    device.payload_stop.value().size())),
                d.GetAllocator());
        }
        if (device.payload_turn_off.has_value())
        {
            d.AddMember("pl_toff",
                Value().SetString(
                    StringRef(device.payload_turn_off.value().data(),
                        device.payload_turn_off.value().size())),
                d.GetAllocator());
        }
        if (device.payload_turn_on.has_value())
        {
            d.AddMember("pl_ton",
                Value().SetString(
                    StringRef(device.payload_turn_on.value().data(),
                        device.payload_turn_on.value().size())),
                d.GetAllocator());
        }
        if (device.schema.has_value())
        {
            d.AddMember("schema",
                Value().SetString(StringRef(device.schema.value().data(),
                    device.schema.value().size())),
                d.GetAllocator());
        }
        if (device.send_command_topic.has_value())
        {
            d.AddMember("send_cmd_t",
                Value().SetString(
                    StringRef(device.send_command_topic.value().data(),
                        device.send_command_topic.value().size())),
                d.GetAllocator());
        }
        if (device.set_fan_speed_topic.has_value())
        {
            d.AddMember("set_fan_spd_t",
                Value().SetString(
                    StringRef(device.set_fan_speed_topic.value().data(),
                        device.set_fan_speed_topic.value().size())),
                d.GetAllocator());
        }
        if (device.unique_id.has_value())
        {
            d.AddMember("uniq_id",
                Value().SetString(StringRef(device.unique_id.value().data(),
                    device.unique_id.value().size())),
                d.GetAllocator());
        }
        if (device.availability.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.availability.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("avty", std::move(a), d.GetAllocator());
        }
        if (device.fan_speed_list.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.fan_speed_list.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("fanspd_lst", std::move(a), d.GetAllocator());
        }
        if (device.supported_features.has_value())
        {
            Value a;
            a.SetArray();
            for (const auto& str : device.supported_features.value())
            {
                a.PushBack(Value().SetString(StringRef(str.data(), str.size())),
                    d.GetAllocator());
            }
            d.AddMember("sup_feat", std::move(a), d.GetAllocator());
        }
        return d;
    }
} // namespace b2h::hass
