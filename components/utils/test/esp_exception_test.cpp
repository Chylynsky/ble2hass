#include "catch2/catch.hpp"

#include <string_view>

#include "utils/esp_exception.hpp"

TEST_CASE("Throw and check message.", "[esp_exception]")
{
    using namespace b2h::utils;
    using namespace std::literals;

    try
    {
        throw esp_exception("Operation failed.", ESP_FAIL);
    }
    catch (const esp_exception& err)
    {
        REQUIRE(std::string_view(err.what()) ==
                "Operation failed. Error code: -1 [ESP_FAIL]"sv);
    }
}

TEST_CASE("Throw and check error code.", "[esp_exception]")
{
    using namespace b2h::utils;

    try
    {
        throw esp_exception("Operation failed.", static_cast<esp_err_t>(4));
    }
    catch (const esp_exception& err)
    {
        REQUIRE(err.errc() == 4);
    }
}
