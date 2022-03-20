#ifndef B2H_HASS_DEVICE_TYPE_HPP
#define B2H_HASS_DEVICE_TYPE_HPP

#include "rapidjson/rapidjson.h"
#include "tcb/span.hpp"

#include <optional>
#include <string_view>

namespace b2h::hass
{
    struct device_type {
        std::optional<std::string_view> manufacturer;
        std::optional<std::string_view> model;
        std::optional<std::string_view> name;
        std::optional<std::string_view> suggested_area;
        std::optional<std::string_view> sw_version;
        std::optional<std::string_view> via_device;
        std::optional<tcb::span<std::string_view>> connections;
        std::optional<tcb::span<std::string_view>> identifiers;
    };
} // namespace b2h::hass

#endif
