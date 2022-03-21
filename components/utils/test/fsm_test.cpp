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
