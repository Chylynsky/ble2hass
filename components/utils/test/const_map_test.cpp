#include "catch2/catch.hpp"

#include <map>
#include <stdexcept>
#include <utility>

#include "utils/const_map.hpp"

TEST_CASE("Check size.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    REQUIRE(m.size() == 6);
}

TEST_CASE("Check if sorted.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = {
        {
            { 1, 1 },
            { 2, 2 },
            { 3, 3 },
            { 4, 4 },
            { 5, 5 },
            { 6, 6 },
        },
    };

    REQUIRE(m.is_sorted());
}

TEST_CASE("Check if not sorted.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 8, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    REQUIRE(!m.is_sorted());
}

TEST_CASE("Check if element exists.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    for (int i = 1; i != 7; ++i)
    {
        REQUIRE(m.contains(i));
    }
}

TEST_CASE("Access elements.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    for (int i = 1; i != 7; ++i)
    {
        REQUIRE(m[i] == i);
    }
}

TEST_CASE("Find element.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    REQUIRE(m.find(3) != m.cend());
}

TEST_CASE("Find non-existing element.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    REQUIRE(m.find(40) == m.cend());
}

TEST_CASE("Throw on \"at\" invocation on non-existing element.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    REQUIRE_THROWS_AS(m.at(40), std::out_of_range);
}

TEST_CASE("Iterate with range-based for.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    int i = 1;

    for (const auto& [key, value] : m)
    {
        REQUIRE(key == value);
        REQUIRE(key == i);
        ++i;
    }
}

TEST_CASE("Copy.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map<int, int, 6> m = { {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 4, 4 },
        { 5, 5 },
        { 6, 6 },
    } };

    std::map<int, int> result(m.cbegin(), m.cend());

    for (const auto& [key, val] : m)
    {
        REQUIRE(val == result[key]);
    }
}

TEST_CASE("Construct using make_const_map and check size.", "[const_map]")
{
    using namespace b2h::utils;

    static constexpr const_map m =
        make_const_map<int, int>(std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3));

    REQUIRE(m.size() == 3);
}
