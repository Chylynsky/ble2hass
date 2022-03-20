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

        using handler_variant_type = std::variant<handler_type<EventsT>...>;

        using handler_array_type =
            std::array<handler_variant_type, EVENT_COUNT>;

        dispatch_table() = default;

        dispatch_table(const dispatch_table&) = default;

        dispatch_table(dispatch_table&&) = default;

        ~dispatch_table() = default;

        dispatch_table& operator=(const dispatch_table&) = default;

        dispatch_table& operator=(dispatch_table&&) = default;

        template<std::size_t EventID, typename HandlerT>
        void set_handler(HandlerT&& handler) noexcept
        {
            static_assert(EventID < sizeof...(EventsT),
                "Event ID out of range.");
            m_dispatch_array[EventID].template emplace<EventID>(
                std::forward<HandlerT>(handler));
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

            const auto fun =
                std::move(std::get<event_id>(m_dispatch_array[event_id]));

            m_dispatch_array[event_id] = {};

            return fun;
        }

    private:
        handler_array_type m_dispatch_array;
    };
} // namespace b2h::event::impl

#endif
