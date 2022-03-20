#ifndef B2H_EVENT_RECEIVER_HPP
#define B2H_EVENT_RECEIVER_HPP

#include <functional>
#include <future>
#include <utility>

namespace b2h::event
{
    template<typename DispatcherT>
    class receiver
    {
    public:
        using dispatcher_type = DispatcherT;

        receiver() = delete;

        explicit receiver(dispatcher_type& disp) :
            m_dispatcher{ std::ref(disp) }
        {
        }

        receiver(const receiver&) = default;

        receiver(receiver&&) = default;

        ~receiver() = default;

        receiver& operator=(const receiver&) = default;

        receiver& operator=(receiver&&) = default;

        template<typename EventT, typename HandlerT>
        void async_receive(HandlerT&& handler) noexcept
        {
            m_dispatcher.get().template shedule_work<EventT>(
                std::forward<HandlerT>(handler));
        }

    private:
        std::reference_wrapper<dispatcher_type> m_dispatcher;
    };

    inline constexpr auto empty_handler = [](auto&&) {
    };

    using empty_handler_t = decltype(empty_handler);

} // namespace b2h::event

#endif
