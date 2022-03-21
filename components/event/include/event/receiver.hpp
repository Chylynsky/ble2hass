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
