#include "catch2/catch.hpp"

#include <stdexcept>

#include "utils/fsm.hpp"

struct state0 {
};
struct state1 {
};
struct state2 {
};

TEST_CASE("Check state.", "[fsm]")
{
    using namespace b2h::utils;

    fsm<state0, state1, state2> sm(state0{});

    REQUIRE(sm.is_state<state0>());
}

TEST_CASE("Get state.", "[fsm]")
{
    using namespace b2h::utils;

    fsm<state0, state1, state2> sm(state0{});
    REQUIRE_NOTHROW(sm.get_state<state0>());
}

TEST_CASE("Get state unchecked.", "[fsm]")
{
    using namespace b2h::utils;

    fsm<state0, state1, state2> sm(state0{});
    REQUIRE_NOTHROW(sm.get_state<state0, no_state_check>());
}

TEST_CASE("Cycle through states.", "[fsm]")
{
    using namespace b2h::utils;

    fsm<state0, state1, state2> sm(state0{});

    REQUIRE(sm.is_state<state0>());
    REQUIRE_NOTHROW(sm.get_state<state0>());
    REQUIRE_NOTHROW(sm.set_state<state1>());
    REQUIRE(sm.is_state<state1>());
    REQUIRE_NOTHROW(sm.get_state<state1>());
    REQUIRE_NOTHROW(sm.set_state<state2>());
    REQUIRE(sm.is_state<state2>());
    REQUIRE_NOTHROW(sm.get_state<state2>());
}

TEST_CASE("Throw on invalid state.", "[fsm]")
{
    using namespace b2h::utils;

    fsm<state0, state1> sm(state0{});

    REQUIRE_THROWS_AS(sm.get_state<state1>(), std::logic_error);
}
