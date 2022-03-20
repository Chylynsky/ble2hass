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
