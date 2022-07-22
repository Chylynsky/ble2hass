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

#ifndef B2H_EVENT_DISPATCH_TABLE_HPP
#define B2H_EVENT_DISPATCH_TABLE_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <variant>

#include "tl/expected.hpp"

#include "event/type_traits.hpp"

namespace b2h::event::impl
{
    template<typename... EventsT>
    class dispatch_table
    {
    public:
        static constexpr std::size_t EVENT_COUNT = sizeof...(EventsT);

        template<typename EventT>
        using argument_type = typename EventT::argument_type;

        template<typename EventT>
        using error_type = typename EventT::error_type;

        template<typename EventT>
        using expected_type =
            tl::expected<argument_type<EventT>, error_type<EventT>>;

        template<typename EventT>
        using handler_type = std::function<void(expected_type<EventT>)>;

        using handler_tuple_type =
            std::tuple<std::optional<handler_type<EventsT>>...>;

        dispatch_table() = default;

        dispatch_table(const dispatch_table&) = delete;

        dispatch_table(dispatch_table&&) = delete;

        ~dispatch_table() = default;

        dispatch_table& operator=(const dispatch_table&) = delete;

        dispatch_table& operator=(dispatch_table&&) = delete;

        template<std::size_t EventID, typename HandlerT>
        void set_handler(HandlerT&& handler) noexcept
        {
            static_assert(EventID < sizeof...(EventsT),
                "Event ID out of range.");
            std::get<EventID>(m_dispatch_table) = std::move(handler);
        }

        template<typename EventT, typename HandlerT>
        void set_handler(HandlerT&& handler) noexcept
        {
            static_assert(is_any_of_v<EventT, EventsT...>,
                "Event type not recognized.");
            static constexpr std::size_t event_id =
                index_of_v<EventT, EventsT...>;
            set_handler<event_id>(std::forward<HandlerT>(handler));
        }

        template<typename EventT>
        [[nodiscard]] auto handler() noexcept
        {
            static_assert(is_any_of_v<EventT, EventsT...>,
                "Event type not recognized.");
            static constexpr std::size_t event_id =
                index_of_v<EventT, EventsT...>;

            auto& handler_ref = std::get<event_id>(m_dispatch_table);
            auto fun          = std::move(*handler_ref);
            handler_ref       = std::nullopt;
            return fun;
        }

    private:
        handler_tuple_type m_dispatch_table;
    };
} // namespace b2h::event::impl

#endif
