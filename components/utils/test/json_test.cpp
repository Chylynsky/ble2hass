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

#include "utils/json.hpp"
#include "catch2/catch.hpp"
#include <string>
#include <string_view>

TEST_CASE("Parse and dump.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{
        "{\"one\":1,\"two\":[1,2],\"three\":[1,2,3]}"
    };

    auto doc = json::parse(input);
    REQUIRE(!json::pred::has_parse_error(doc));
    REQUIRE(input == json::dump(doc));
}

TEST_CASE("Check is_object predicate on object.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "{\"one\":1}" };

    REQUIRE(json::pred::is_object(json::parse(input)));
}

TEST_CASE("Check is_object predicate on non-object.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "1" };

    REQUIRE(!json::pred::is_object(json::parse(input)));
}

TEST_CASE("Check is_array predicate on array.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "[1,2,3]" };

    REQUIRE(json::pred::is_array(json::parse(input)));
}

TEST_CASE("Check is_array predicate on non-array.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "1" };

    REQUIRE(!json::pred::is_array(json::parse(input)));
}

TEST_CASE("Check if key exists on existing key.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "{\"one\":1}" };

    const auto has_key = json::pred::get_has_key("one");

    REQUIRE(has_key(json::parse(input)));
}

TEST_CASE("Check if key exists on non-existing key.", "[json]")
{
    using namespace b2h::utils;

    constexpr std::string_view input{ "{\"one\":1}" };

    const auto has_key = json::pred::get_has_key("two");

    REQUIRE(!has_key(json::parse(input)));
}
