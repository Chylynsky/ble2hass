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

#include "utils/mac.hpp"
#include "catch2/catch.hpp"
#include "tcb/span.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

TEST_CASE("Create from std::string.", "[mac]")
{
    using namespace b2h;

    std::string s{ "ab:0d:53:a1:ff:65" };
    utils::mac m = utils::make_mac(s);

    REQUIRE(m.to_string() == s);
}

TEST_CASE("Create from uint8_t buffer.", "[mac]")
{
    using namespace b2h;

    std::vector<std::uint8_t> v{ 0x65, 0x12, 0x6F, 0x00, 0xAA, 0xFF };
    utils::mac m = utils::make_mac(v.cbegin(), v.cend()).value();

    REQUIRE(m.to_string() == "ff:aa:00:6f:12:65");
}

TEST_CASE("Dump as char buffer.", "[mac]")
{
    using namespace b2h;

    std::string_view s{ "ab:0d:53:a1:ff:65" };
    std::array<char, utils::mac::MAC_STR_SIZE> buff;
    utils::mac m = utils::make_mac(s);

    m.to_charbuf(buff.begin(), buff.end());

    REQUIRE(std::string_view(buff.data(), buff.size()) == s);
}
