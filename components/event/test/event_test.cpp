#include "catch2/catch.hpp"

#include <functional>
#include <future>
#include <stdexcept>

#include "event/event.hpp"

struct event1 : public b2h::event::basic_event<int, int> {
};

struct event2 : public b2h::event::basic_event<int, int> {
};

struct event3 : public b2h::event::basic_event<int, int> {
};

template<typename ReceiverT>
class recursive_sheduler
{
public:
    recursive_sheduler(ReceiverT&& receiver) :
        m_receiver{ std::move(receiver) },
        m_invoked{ 0 }
    {
    }

    template<typename EventT>
    void shedule(const std::size_t times)
    {
        m_receiver.template async_receive<EventT>(
            [this, times](typename EventT::expected_type) {
                assert(times != 0);
                ++m_invoked;
                if (m_invoked != times)
                {
                    shedule<EventT>(times);
                }
            });
    }

    std::size_t invoked() const noexcept
    {
        return m_invoked;
    }

private:
    ReceiverT m_receiver;
    std::size_t m_invoked;
};

TEST_CASE("Event IDs.", "[event]")
{
    using namespace b2h::event;
    using dispatcher_t = dispatcher<event1, event2, event3>;
    REQUIRE(dispatcher_t::event_id<event1>() == 0);
    REQUIRE(dispatcher_t::event_id<event2>() == 1);
    REQUIRE(dispatcher_t::event_id<event3>() == 2);
}

TEST_CASE("Exit when no work requested.", "[event]")
{
    using namespace b2h::event;
    context ctx{};
    ctx.run();
}

TEST_CASE("Dispatch event nobody listens to.", "[event]")
{
    using namespace b2h::event;
    using dispatcher_t = dispatcher<event1, event2, event3>;

    context ctx{};
    dispatcher_t disp{ ctx };
    auto rcv = disp.make_receiver();

    rcv.async_receive<event1>([](auto&&) {});

    {
        auto ctx_task = std::async(std::launch::async, [&]() { ctx.run(); });
        disp.async_dispatch<event2>(0); // Should be ignored
        disp.async_dispatch<event3>(0); // Should be ignored

        disp.async_dispatch<event1>(0); // Should after this dispatch
    }
}

TEST_CASE("Dispatch event.", "[event]")
{
    using namespace b2h::event;
    using dispatcher_t = dispatcher<event1>;

    context ctx{};
    dispatcher_t disp{ ctx };
    auto rcv     = disp.make_receiver();
    bool invoked = false;

    rcv.async_receive<event1>([&](tl::expected<int, int> arg) {
        REQUIRE(arg.has_value());
        REQUIRE(arg.value() == 6);
        invoked = true;
    });

    {
        auto ctx_task = std::async(std::launch::async, [&]() { ctx.run(); });
        disp.async_dispatch<event1>(6);
    }

    REQUIRE(invoked);
}

TEST_CASE("Dispatch multiple events.", "[event]")
{
    using namespace b2h::event;
    using dispatcher_t = dispatcher<event1, event2, event3>;

    context ctx{};
    dispatcher_t disp{ ctx };
    auto rcv = disp.make_receiver();

    rcv.async_receive<event1>([](tl::expected<int, int> arg) {
        REQUIRE(arg.has_value());
        REQUIRE(arg.value() == 1);
    });

    rcv.async_receive<event2>([](tl::expected<int, int> arg) {
        REQUIRE(arg.has_value());
        REQUIRE(arg.value() == 2);
    });

    rcv.async_receive<event3>([](tl::expected<int, int> arg) {
        REQUIRE(arg.has_value());
        REQUIRE(arg.value() == 3);
    });

    {
        auto ctx_task = std::async(std::launch::async, [&]() { ctx.run(); });
        disp.async_dispatch<event2>(2);
        disp.async_dispatch<event3>(3);
        disp.async_dispatch<event1>(1);
    }
}

TEST_CASE("Throw in handler.", "[event]")
{
    using namespace b2h::event;
    using dispatcher_t = dispatcher<event1>;

    context ctx{};

    auto disp = dispatcher_t{ ctx };
    auto rcv  = disp.make_receiver();

    bool invoked = false;

    rcv.async_receive<event1>([&](event1::expected_type arg) {
        invoked = true;
        throw std::runtime_error("Should be thrown by context::run().");
    });

    {
        auto fut = std::async(std::launch::async, [&]() {
            REQUIRE_THROWS_AS(std::invoke([&]() { ctx.run(); }),
                std::runtime_error);
        });

        disp.async_dispatch<event1>(tl::expected<int, int>{ 0 });
    }

    REQUIRE(invoked);
}
