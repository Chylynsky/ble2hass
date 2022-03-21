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

#include "benchmark/benchmark.h"

#include <chrono>
#include <thread>

#include "event/event.hpp"

// 2000ns
// 1900ns

struct event0 : public b2h::event::basic_event<int, int> {
};

struct event1 : public b2h::event::basic_event<int, int> {
};

struct event2 : public b2h::event::basic_event<int, int> {
};

struct event3 : public b2h::event::basic_event<int, int> {
};

struct event4 : public b2h::event::basic_event<int, int> {
};

struct event5 : public b2h::event::basic_event<int, int> {
};

static void event_dispatch(benchmark::State& state)
{
    using namespace std::literals;
    using namespace b2h::event;
    using dispatcher_t =
        dispatcher<event0, event1, event2, event3, event4, event5>;

    context ctx{};
    dispatcher_t disp{ ctx };
    auto rcv = disp.make_receiver();

    rcv.async_receive<event0>([](tl::expected<int, int>) {});
    auto ctx_task = std::async(std::launch::async, [&]() { ctx.run(); });

    for (auto _ : state)
    {
        rcv.async_receive<event1>([](tl::expected<int, int>) {});
        rcv.async_receive<event2>([](tl::expected<int, int>) {});
        rcv.async_receive<event3>([](tl::expected<int, int>) {});
        rcv.async_receive<event4>([](tl::expected<int, int>) {});
        rcv.async_receive<event5>([](tl::expected<int, int>) {});
        disp.async_dispatch<event1>(0);
        disp.async_dispatch<event2>(0);
        disp.async_dispatch<event3>(0);
        disp.async_dispatch<event4>(0);
        disp.async_dispatch<event5>(0);
    }

    disp.async_dispatch<event0>(0);
}
BENCHMARK(event_dispatch);
