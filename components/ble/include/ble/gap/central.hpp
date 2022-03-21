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

#ifndef B2H_BLE_CENTRAL_HPP
#define B2H_BLE_CENTRAL_HPP

#include "host/ble_gap.h"

#undef min
#undef max

#include <chrono>
#include <string_view>

#include "tl/expected.hpp"

#include "event/event.hpp"
#include "utils/logger.hpp"
#include "utils/mac.hpp"

namespace b2h
{
    namespace events::ble::gap
    {
        struct connect_args_t {
            std::uint16_t connection_handle;
        };

        struct connect : public event::basic_event<connect_args_t, int> {
        };

        struct notify_rx_args {
            std::uint16_t connection_handle;
            std::uint16_t attribute_handle;
            std::vector<std::uint8_t> data;
        };

        struct notify_rx : public event::basic_event<notify_rx_args, int> {
        };

        struct on_disconnect : public event::basic_event<std::uint16_t, int> {
        };
    } // namespace events::ble::gap

    namespace ble::gap
    {
        namespace impl
        {
            int blecent_gap_event(::ble_gap_event* event, void* arg) noexcept;
        }

        class central
        {
        public:
            // clang-format off
            using dispatcher_type = event::dispatcher<
                events::ble::gap::connect,
                events::ble::gap::notify_rx,
                events::ble::gap::on_disconnect
            >;
            // clang-format on
            using receiver_type = dispatcher_type::receiver_type;

            central(event::context& ctx) noexcept;

            central() = delete;

            central(const central&) = delete;

            central(central&& other) noexcept;

            ~central() = default;

            central& operator=(const central&) = delete;

            central& operator=(central&& other) noexcept;

            template<typename HandlerT, typename Rep,
                typename Period = std::ratio<1>>
            void async_connect(const utils::mac& addr,
                std::chrono::duration<Rep, Period> timeout, HandlerT&& handler)
            {
                namespace events = events::ble::gap;

                const ::ble_addr_t addr_internal{
                    BLE_ADDR_PUBLIC,
                    {
                        *addr.as_bytes(),
                        *(addr.as_bytes() + 1),
                        *(addr.as_bytes() + 2),
                        *(addr.as_bytes() + 3),
                        *(addr.as_bytes() + 4),
                        *(addr.as_bytes() + 5),
                    },
                };

                log::debug(COMPONENT, "Connecting to {}.", addr.to_string());

                m_receiver.async_receive<events::connect>(
                    std::forward<HandlerT>(handler));

                std::uint8_t own_addr_type = BLE_OWN_ADDR_PUBLIC;
                int rc = ble_hs_id_infer_auto(0, &own_addr_type);
                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to infer own address. Error code: [{}]",
                        rc);
                    m_dispatcher.async_dispatch<events::connect>(
                        tl::make_unexpected(rc));
                    return;
                }

                rc = ::ble_gap_connect(own_addr_type,
                    &addr_internal,
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        timeout)
                        .count(),
                    nullptr,
                    &impl::blecent_gap_event,
                    this);
                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to connect to device. Error code: [{}]",
                        rc);
                    m_dispatcher.async_dispatch<events::connect>(
                        tl::make_unexpected(rc));
                    return;
                }
            }

            template<typename HandlerT>
            void async_notify_rx(HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gap;

                log::debug(COMPONENT,
                    "Setting callback to receive notification.");

                m_receiver.async_receive<events::notify_rx>(
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void on_disconnect(HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gap;

                log::debug(COMPONENT,
                    "Setting callback to receive disconnect event.");

                m_receiver.async_receive<events::on_disconnect>(
                    std::forward<HandlerT>(handler));
            }

        private:
            friend int impl::blecent_gap_event(
                ::ble_gap_event* event, void* arg) noexcept;

            static constexpr std::string_view COMPONENT{ "gap::central" };

            dispatcher_type m_dispatcher;
            receiver_type m_receiver;

            std::vector<std::uint8_t> m_notify_buffer;
        };
    } // namespace ble::gap
} // namespace b2h

#endif
