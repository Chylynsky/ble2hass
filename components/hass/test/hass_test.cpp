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
