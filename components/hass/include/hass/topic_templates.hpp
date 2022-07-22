#ifndef B2H_HASS_TOPIC_TEMPLATES_HPP
#define B2H_HASS_TOPIC_TEMPLATES_HPP

#include <string_view>

namespace b2h::hass
{
    inline constexpr std::string_view SENSOR_CFG_TOPIC_TMPL{
        "homeassistant/sensor/{}/config"
    };

    inline constexpr std::string_view SENSOR_STATE_TOPIC_TMPL{
        "homeassistant/sensor/{}/state"
    };

    inline constexpr std::string_view NUMBER_CFG_TOPIC_TMPL{
        "homeassistant/number/{}/config"
    };

    inline constexpr std::string_view NUMBER_STATE_TOPIC_TMPL{
        "homeassistant/number/{}/state"
    };

    inline constexpr std::string_view NUMBER_CMD_TOPIC_TMPL{
        "homeassistant/number/{}/state"
    };

    inline constexpr std::string_view SELECT_CFG_TOPIC_TMPL{
        "homeassistant/select/{}/config"
    };

    inline constexpr std::string_view SELECT_STATE_TOPIC_TMPL{
        "homeassistant/select/{}/state"
    };

    inline constexpr std::string_view SELECT_CMD_TOPIC_TMPL{
        "homeassistant/select/{}/cmd"
    };

    inline constexpr std::string_view SWITCH_CFG_TOPIC_TMPL{
        "homeassistant/switch/{}/config"
    };

    inline constexpr std::string_view SWITCH_STATE_TOPIC_TMPL{
        "homeassistant/switch/{}/state"
    };

    inline constexpr std::string_view SWITCH_CMD_TOPIC_TMPL{
        "homeassistant/switch/{}/cmd"
    };
} // namespace b2h::hass

#endif
