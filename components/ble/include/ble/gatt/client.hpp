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

#ifndef B2H_BLE_GATT_CLIENT_HPP
#define B2H_BLE_GATT_CLIENT_HPP

#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#undef min
#undef max

#include <algorithm>
#include <string_view>

#include "tcb/span.hpp"

#include "event/basic_event.hpp"
#include "event/event.hpp"
#include "utils/logger.hpp"
#include "utils/mac.hpp"

namespace b2h
{
    namespace ble::gatt
    {
        using service        = ::ble_gatt_svc;
        using characteristic = ::ble_gatt_chr;
        using descriptor     = ::ble_gatt_dsc;
        using attribute      = ::ble_gatt_attr;
    } // namespace ble::gatt

    namespace events::ble::gatt
    {
        namespace impl
        {
            template<typename ArgT>
            struct gatt_event :
                public event::basic_event<ArgT, ble_gatt_error> {
            };
        } // namespace impl

        struct discover_services :
            public impl::gatt_event<tcb::span<b2h::ble::gatt::service>> {
        };

        struct discover_characteristics :
            public impl::gatt_event<tcb::span<b2h::ble::gatt::characteristic>> {
        };

        struct discover_descriptors :
            public impl::gatt_event<tcb::span<b2h::ble::gatt::descriptor>> {
        };

        struct read : public impl::gatt_event<tcb::span<std::uint8_t>> {
        };

        struct write : public impl::gatt_event<void> {
        };

    } // namespace events::ble::gatt

    namespace ble::gatt
    {
        namespace impl
        {
            struct client_state {
                // clang-format off
                using dispatcher_type = event::dispatcher<
                    events::ble::gatt::discover_services,
                    events::ble::gatt::discover_characteristics,
                    events::ble::gatt::discover_descriptors,
                    events::ble::gatt::read,
                    events::ble::gatt::write
                >;
                // clang-format on
                using receiver_type = typename dispatcher_type::receiver_type;

                client_state(event::context& context,
                    const std::uint16_t connection_handle,
                    const utils::mac& mac) :
                    m_dispatcher{ context },
                    m_receiver{ m_dispatcher.make_receiver() },
                    m_connection_handle{ connection_handle },
                    m_mac{ mac }
                {
                }

                dispatcher_type m_dispatcher;
                receiver_type m_receiver;

                std::uint16_t m_connection_handle;
                utils::mac m_mac;

                std::vector<std::uint8_t> m_read_buffer = {};

                std::vector<service> m_service_cache               = {};
                std::vector<characteristic> m_characteristic_cache = {};
                std::vector<descriptor> m_descriptor_cache         = {};
            };
        } // namespace impl

        inline constexpr std::string_view COMPONENT{ "gatt::client" };

        class client final
        {
        public:
            client(event::context& context,
                const std::uint16_t connection_handle,
                const utils::mac& mac) noexcept;

            client(const client&) = delete;

            client(client&&) = default;

            ~client() = default;

            client& operator=(const client&) = delete;

            client& operator=(client&&) = default;

            std::uint16_t connection_handle() const noexcept
            {
                return m_state->m_connection_handle;
            }

            const utils::mac& mac() const noexcept
            {
                return m_state->m_mac;
            }

            template<typename HandlerT>
            void async_discover_services(HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT, "Discovering services.");

                m_state->m_receiver.async_receive<events::discover_services>(
                    std::forward<HandlerT>(handler));

                m_state->m_service_cache.clear();

                int rc = ::ble_gattc_disc_all_svcs(m_state->m_connection_handle,
                    &discover_services_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover all services procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_services>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_discover_service_by_uuid(
                const ble_uuid_t* uuid, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT, "Discovering services by uuid.");

                m_state->m_receiver.async_receive<events::discover_services>(
                    std::forward<HandlerT>(handler));

                m_state->m_service_cache.clear();

                int rc =
                    ::ble_gattc_disc_svc_by_uuid(m_state->m_connection_handle,
                        uuid,
                        &discover_services_callback,
                        static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover all services procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_services>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_discover_characteristics(
                const service& srv, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT,
                    "Discovering characteristics, start handle: {}, end_handle: {}.",
                    srv.start_handle,
                    srv.end_handle);

                m_state->m_receiver
                    .async_receive<events::discover_characteristics>(
                        std::forward<HandlerT>(handler));

                m_state->m_characteristic_cache.clear();

                int rc = ::ble_gattc_disc_all_chrs(m_state->m_connection_handle,
                    srv.start_handle,
                    srv.end_handle,
                    &discover_characteristics_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover all characteristics procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_characteristics>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_discover_characteristic_by_uuid(const service& srv,
                const ble_uuid_t* uuid, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT,
                    "Discovering characteristics by UUID, start handle: {}, end_handle: {}.",
                    srv.start_handle,
                    srv.end_handle);

                m_state->m_receiver
                    .async_receive<events::discover_characteristics>(
                        std::forward<HandlerT>(handler));

                m_state->m_characteristic_cache.clear();

                int rc =
                    ::ble_gattc_disc_chrs_by_uuid(m_state->m_connection_handle,
                        srv.start_handle,
                        srv.end_handle,
                        uuid,
                        &discover_characteristics_callback,
                        static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover characteristics by uuid procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_characteristics>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_discover_descriptors(
                const characteristic& chr, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                // Find characteristic end handle
                const auto chr_iter =
                    std::find_if(m_state->m_characteristic_cache.cbegin(),
                        m_state->m_characteristic_cache.cend(),
                        [handle = chr.val_handle](const characteristic& chr) {
                            return handle == chr.val_handle;
                        });

                // If the obtains the characteristic, it must be in the cache.
                assert(chr_iter != m_state->m_characteristic_cache.end());

                std::uint16_t chr_end_handle = chr.def_handle;

                if (const auto next_chr_iter = std::next(chr_iter);
                    next_chr_iter != m_state->m_characteristic_cache.cend())
                {
                    chr_end_handle = next_chr_iter->def_handle - 1;
                }
                else
                {
                    // Find service which contains the current characteristic
                    const auto srv_iter =
                        std::find_if(m_state->m_service_cache.cbegin(),
                            m_state->m_service_cache.cend(),
                            [handle = chr.val_handle](const service& srv) {
                                return (handle > srv.start_handle &&
                                        handle < srv.end_handle);
                            });

                    assert(srv_iter != m_state->m_service_cache.cend());

                    chr_end_handle = srv_iter->end_handle;
                }

                log::debug(COMPONENT,
                    "Discovering descriptors, start handle: {}, end_handle: {}.",
                    chr.val_handle,
                    chr_end_handle);

                m_state->m_receiver.async_receive<events::discover_descriptors>(
                    std::forward<HandlerT>(handler));

                m_state->m_descriptor_cache.clear();

                int rc = ::ble_gattc_disc_all_dscs(m_state->m_connection_handle,
                    chr.val_handle,
                    chr_end_handle,
                    &discover_descriptors_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover all descriptors procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_descriptors>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_discover_descriptors(
                const service& srv, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT,
                    "Discovering descriptors, start handle: {}, end_handle: {}.",
                    srv.start_handle,
                    srv.end_handle);

                m_state->m_receiver.async_receive<events::discover_descriptors>(
                    std::forward<HandlerT>(handler));

                m_state->m_descriptor_cache.clear();

                int rc = ::ble_gattc_disc_all_dscs(m_state->m_connection_handle,
                    srv.start_handle,
                    srv.end_handle,
                    &discover_descriptors_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate discover all descriptors procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher
                        .async_dispatch<events::discover_descriptors>(
                            tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename HandlerT>
            void async_read(
                std::uint16_t attr_handle, HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT,
                    "Reading attribute value, handle: {}.",
                    attr_handle);

                m_state->m_receiver.async_receive<events::read>(
                    std::forward<HandlerT>(handler));

                int rc = ::ble_gattc_read(m_state->m_connection_handle,
                    attr_handle,
                    &attribute_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate read procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher.async_dispatch<events::read>(
                        tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            template<typename ContainerT, typename HandlerT>
            void async_write(std::uint16_t attr_handle, const ContainerT& buff,
                HandlerT&& handler) noexcept
            {
                namespace events = events::ble::gatt;

                log::debug(COMPONENT,
                    "Writing to attribute, handle: {}.",
                    attr_handle);

                m_state->m_receiver.async_receive<events::write>(
                    std::forward<HandlerT>(handler));

                int rc = ::ble_gattc_write_flat(m_state->m_connection_handle,
                    attr_handle,
                    buff.data(),
                    buff.size(),
                    &attribute_callback,
                    static_cast<void*>(m_state.get()));

                if (rc != 0)
                {
                    log::error(COMPONENT,
                        "Failed to initiate write procedure. Error code: {}.",
                        rc);
                    m_state->m_dispatcher.async_dispatch<events::write>(
                        tl::make_unexpected(ble_gatt_error{}));
                    return;
                }
            }

            void terminate()
            {
                ble_gap_terminate(m_state->m_connection_handle,
                    BLE_ERR_REM_USER_CONN_TERM);
            }

        private:
            std::unique_ptr<impl::client_state> m_state;

            static int discover_services_callback(std::uint16_t conn_handle,
                const ble_gatt_error* error, const service* svc, void* arg);

            static int discover_characteristics_callback(
                std::uint16_t conn_handle, const ble_gatt_error* error,
                const characteristic* service, void* arg);

            static int discover_descriptors_callback(std::uint16_t conn_handle,
                const ble_gatt_error* error,
                std::uint16_t characteristic_handle, const descriptor* service,
                void* arg);

            static int attribute_callback(std::uint16_t conn_handle,
                const ble_gatt_error* error, attribute* attr, void* arg);

            template<typename AttributeT>
            static auto uuid_equal_to(const ble_uuid_t* uuid)
            {
                return [=](const auto& attr) {
                    return ::ble_uuid_cmp(&attr.uuid.u, uuid) == 0;
                };
            }
        };
    } // namespace ble::gatt
} // namespace b2h

#endif
