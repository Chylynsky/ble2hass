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

#ifndef B2H_EVENT_DISPATCHER_HPP
#define B2H_EVENT_DISPATCHER_HPP

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <functional>
#include <iostream>
#include <thread>
#include <type_traits>
#include <utility>

#include "fmt/ranges.h"

#include "event/dispatch_table.hpp"
#include "event/receiver.hpp"
#include "event/type_traits.hpp"

#include "utils/logger.hpp"

namespace b2h::event
{
    template<typename ContextT, typename... EventsT>
    class basic_dispatcher
    {
    public:
        using context_type  = ContextT;
        using receiver_type = receiver<basic_dispatcher>;

        basic_dispatcher() = delete;

        explicit basic_dispatcher(context_type& context) :
            m_context{ std::ref(context) },
            m_flags{},
            m_dispatch_table{}
        {
        }

        basic_dispatcher(const basic_dispatcher&) = delete;

        basic_dispatcher(basic_dispatcher&&) = default;

        ~basic_dispatcher() = default;

        basic_dispatcher& operator=(const basic_dispatcher&) = default;

        basic_dispatcher& operator=(basic_dispatcher&&) = default;

        template<typename EventT>
        static constexpr std::size_t event_id() noexcept
        {
            return index_of_v<EventT, EventsT...>;
        }

        receiver_type make_receiver() noexcept
        {
            return receiver_type{ *this };
        }

        context_type& context() noexcept
        {
            return m_context.get();
        }

        template<typename EventT, typename HandlerT>
        void shedule_work(HandlerT&& handler) noexcept
        {
            static_assert(
                std::is_invocable_v<HandlerT, typename EventT::expected_type>,
                "Invalid handler type.");

            static constexpr std::size_t id = event_id<EventT>();

            log::verbose(COMPONENT,
                "Sheduling work for event id: {}. Current event flags: {}",
                id,
                m_flags);

            assert(!m_flags[id]); // Handler should not be already waiting
                                  // for completion
            m_dispatch_table.template set_handler<id>(
                std::forward<HandlerT>(handler));
            m_flags[id] = true;
            ++m_context.get().active_events();
        }

        template<typename EventT>
        void async_dispatch(typename EventT::expected_type&& expected) noexcept
        {
            static constexpr std::size_t id = event_id<EventT>();

            log::verbose(COMPONENT,
                "Dispatching handler for event id: {}. Current event flags: {}",
                id,
                m_flags);

            if (!m_flags[id])
            {
                // Nobody is listening, ignore the event
                log::verbose(COMPONENT, "Event with id {} ignored.", id);
                return;
            }

            auto handler =
                std::move(m_dispatch_table.template handler<EventT>());

            // Ready for new work to be sheduled
            m_flags[id] = false;

            m_context.get().shedule(
                [&active_events = m_context.get().active_events(),
                    handler{ std::move(handler) },
                    arg{ std::move(expected) }]() {
                    --active_events;
                    handler(std::move(arg));
                });
        }

    private:
        using dispatch_table_type = impl::dispatch_table<EventsT...>;

        static constexpr std::string_view COMPONENT{ "event::dispatcher" };

        std::reference_wrapper<context_type> m_context;
        std::array<bool, sizeof...(EventsT)> m_flags;
        dispatch_table_type m_dispatch_table;
    };
} // namespace b2h::event

#endif
