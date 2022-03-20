#include "catch2/catch.hpp"

#include <string>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "hass/device_types.hpp"

namespace rjs = rapidjson;

static std::string dump(const rjs::Document& document) noexcept
{
    rjs::StringBuffer buffer;
    rjs::Writer<rjs::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string result{ buffer.GetString(), buffer.GetSize() };
    return result;
}

TEST_CASE("Conversion to JSON.", "[hass]")
{
    using namespace b2h::hass;
    camera_type dev;
    dev.topic   = "example/topic";
    auto result = serialize(dev);
    REQUIRE(dump(result) == "{\"t\":\"example/topic\"}");
}

TEST_CASE("Conversion to JSON with optional.", "[hass]")
{
    using namespace b2h::hass;
    binary_sensor_type dev;
    dev.state_topic  = "example/topic";
    dev.force_update = true;
    auto result      = serialize(dev);
    REQUIRE(dump(result) == "{\"stat_t\":\"example/topic\",\"frc_upd\":true}");
}
