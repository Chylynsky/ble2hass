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
