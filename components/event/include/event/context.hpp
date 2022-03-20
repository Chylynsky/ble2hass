#ifndef B2H_EVENT_ECONTEXT_HPP
#define B2H_EVENT_CONTEXT_HPP

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

#include "utils/logger.hpp"

namespace b2h::event
{
    class basic_context
    {
    public:
        static_assert(std::atomic<std::size_t>::is_always_lock_free,
            "std::atomic<std::size_t> not always lock free.");

        basic_context() noexcept :
            m_mutex{},
            m_cv{},
            m_dispatch_queue{},
            m_active_events{ 0ULL }
        {
        }

        basic_context(const basic_context&) = delete;

        basic_context(basic_context&&) = default;

        ~basic_context() = default;

        basic_context& operator=(const basic_context&) = delete;

        basic_context& operator=(basic_context&&) = default;

        [[nodiscard]] std::atomic<std::size_t>& active_events() noexcept
        {
            return m_active_events;
        }

        template<typename FuncT>
        void shedule(FuncT&& func) noexcept
        {
            {
                std::lock_guard<std::mutex> lock{ m_mutex };
                m_dispatch_queue.emplace(std::forward<FuncT>(func));
            }
            m_cv.notify_one();
        }

        void run()
        {
            std::function<void(void)> func;

            while (m_active_events)
            {
                log::verbose(COMPONENT,
                    "Current active events: {}.",
                    m_active_events.load());

                {
                    std::unique_lock<std::mutex> lock{ m_mutex };
                    while (m_dispatch_queue.empty())
                    {
                        log::verbose(COMPONENT, "Context idle.");
                        m_cv.wait(lock);
                    }

                    func = std::move(m_dispatch_queue.front());
                    m_dispatch_queue.pop();
                }

                log::verbose(COMPONENT,
                    "Calling event handler.",
                    m_active_events.load());
                func();
            }
        }

    private:
        static constexpr std::string_view COMPONENT{ "event::context" };

        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::queue<std::function<void(void)>> m_dispatch_queue;
        std::atomic<std::size_t> m_active_events;
    };
} // namespace b2h::event

#endif
