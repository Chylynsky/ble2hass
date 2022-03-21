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
