#include "host/ble_hs.h"
#include "os/os_mbuf.h"
#undef min
#undef max

#include "ble/gatt/client.hpp"

namespace b2h::ble::gatt
{
    client::client(event::context& context,
        const std::uint16_t connection_handle, const utils::mac& mac) noexcept :
        m_state{ std::make_unique<impl::client_state>(
            context, connection_handle, mac) }
    {
    }

    int client::discover_services_callback(std::uint16_t conn_handle,
        const ble_gatt_error* error, const service* svc, void* arg)
    {
        namespace events = events::ble::gatt;

        impl::client_state* client_ptr = static_cast<impl::client_state*>(arg);
        assert(client_ptr->m_connection_handle == conn_handle);

        if (error->status == BLE_HS_EDONE)
        {
            log::info(COMPONENT, "Successfully discovered services.");

            log::verbose(COMPONENT, "Discovered services:");
            for (const auto& srv : client_ptr->m_service_cache)
            {
                [[maybe_unused]] std::array<char, 40> buff;
                log::verbose(COMPONENT,
                    "start_handle: {}, end_handle: {}, uuid: {}",
                    srv.start_handle,
                    srv.end_handle,
                    ::ble_uuid_to_str(&srv.uuid.u, buff.data()));
            }

            client_ptr->m_dispatcher.async_dispatch<events::discover_services>(
                tcb::make_span(client_ptr->m_service_cache));
            return 0;
        }
        else if (error->status != 0)
        {
            log::error(COMPONENT,
                "Failed to discover services, error code: {}.",
                error->status);

            client_ptr->m_dispatcher.async_dispatch<events::discover_services>(
                tl::make_unexpected(*error));
            return -1;
        }

        client_ptr->m_service_cache.emplace_back(*svc);

        return 0;
    }

    int client::discover_characteristics_callback(std::uint16_t conn_handle,
        const ble_gatt_error* error, const characteristic* chr, void* arg)
    {
        namespace events = events::ble::gatt;

        impl::client_state* client_ptr = static_cast<impl::client_state*>(arg);
        assert(client_ptr->m_connection_handle == conn_handle);

        if (error->status == BLE_HS_EDONE)
        {
            log::info(COMPONENT, "Successfully discovered characteristics.");

            log::verbose(COMPONENT, "Discovered characteristics:");
            for (const auto& chr : client_ptr->m_characteristic_cache)
            {
                [[maybe_unused]] std::array<char, 40> buff;
                log::verbose(COMPONENT,
                    "def_handle: {}, val_handle: {}, uuid: {}",
                    chr.def_handle,
                    chr.val_handle,
                    ::ble_uuid_to_str(&chr.uuid.u, buff.data()));
            }

            client_ptr->m_dispatcher
                .async_dispatch<events::discover_characteristics>(
                    tcb::make_span(client_ptr->m_characteristic_cache));
            return 0;
        }
        else if (error->status != 0)
        {
            log::error(COMPONENT,
                "Failed to discover characteristics, error code: {}.",
                error->status);
            client_ptr->m_dispatcher
                .async_dispatch<events::discover_characteristics>(
                    tl::make_unexpected(*error));
            return -1;
        }

        client_ptr->m_characteristic_cache.emplace_back(*chr);

        return 0;
    }

    int client::discover_descriptors_callback(std::uint16_t conn_handle,
        const ble_gatt_error* error, std::uint16_t characteristic_handle,
        const descriptor* desc, void* arg)
    {
        namespace events = events::ble::gatt;

        impl::client_state* client_ptr = static_cast<impl::client_state*>(arg);
        assert(client_ptr->m_connection_handle == conn_handle);

        if (error->status == BLE_HS_EDONE)
        {
            log::info(COMPONENT, "Successfully discovered descriptors.");

            log::verbose(COMPONENT, "Discovered descriptors:");
            for (const auto& desc : client_ptr->m_descriptor_cache)
            {
                [[maybe_unused]] std::array<char, 40> buff;
                log::verbose(COMPONENT,
                    "handle: {}, uuid: {}",
                    desc.handle,
                    ::ble_uuid_to_str(&desc.uuid.u, buff.data()));
            }

            client_ptr->m_dispatcher
                .async_dispatch<events::discover_descriptors>(
                    tcb::make_span(client_ptr->m_descriptor_cache));
            return 0;
        }
        else if (error->status != 0)
        {
            log::error(COMPONENT,
                "Failed to discover descriptors, error code: {}.",
                error->status);
            client_ptr->m_dispatcher
                .async_dispatch<events::discover_descriptors>(
                    tl::make_unexpected(*error));
            return -1;
        }

        client_ptr->m_descriptor_cache.emplace_back(*desc);

        return 0;
    }

    int client::attribute_callback(std::uint16_t conn_handle,
        const ble_gatt_error* error, attribute* attr, void* arg)
    {
        namespace events = events::ble::gatt;

        impl::client_state* client_ptr = static_cast<impl::client_state*>(arg);
        assert(client_ptr->m_connection_handle == conn_handle);

        if (error->status != 0)
        {
            log::error(COMPONENT,
                "Failed to perform attribute action, error code: {}.",
                error->status);
            client_ptr->m_dispatcher.async_dispatch<events::read>(
                tl::make_unexpected(*error));
            client_ptr->m_dispatcher.async_dispatch<events::write>(
                tl::make_unexpected(*error));
            return -1;
        }

        // attr->om_pkthdr_len != 0 checks if buffer is a normal buffer
        if (attr != nullptr && attr->om && attr->om->om_pkthdr_len != 0)
        {
            client_ptr->m_read_buffer.resize(attr->om->om_len);

            ::os_mbuf_copydata(attr->om,
                0,
                attr->om->om_len,
                static_cast<void*>(client_ptr->m_read_buffer.data()));

            client_ptr->m_dispatcher.async_dispatch<events::read>(
                tcb::make_span(client_ptr->m_read_buffer));

            log::info(COMPONENT,
                "Successfully performed read attribute action.");
            return 0;
        }

        client_ptr->m_dispatcher.async_dispatch<events::write>({});
        log::info(COMPONENT, "Successfully performed write attribute action.");
        return 0;
    }
} // namespace b2h::ble::gatt
