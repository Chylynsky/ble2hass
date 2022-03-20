#include "esp_nimble_hci.h"
#include "os/os_mbuf.h"
#undef min
#undef max

#include "ble/gap/central.hpp"

namespace b2h::ble::gap
{
    namespace impl
    {
        int blecent_gap_event(::ble_gap_event* event, void* arg) noexcept
        {
            namespace events = events::ble::gap;

            // ble_gap_conn_desc desc;
            central* const cent = reinterpret_cast<central*>(arg);

            switch (event->type)
            {
            case BLE_GAP_EVENT_CONNECT:
                log::debug(central::COMPONENT,
                    "GAP event: BLE_GAP_EVENT_CONNECT.");

                if (event->connect.status == 0)
                {
                    log::info(central::COMPONENT, "Connection established.");
                    cent->m_dispatcher.async_dispatch<events::connect>(
                        events::connect::expected_type{
                            events::connect_args_t{
                                event->connect.conn_handle,
                            },
                        });
                }
                else
                {
                    // Connection attempt failed; resume scanning.
                    log::error(central::COMPONENT,
                        "Connection failed, status: {}.",
                        event->connect.status);
                    cent->m_dispatcher.async_dispatch<events::connect>(
                        tl::make_unexpected(event->connect.status));
                }
                return 0;
            case BLE_GAP_EVENT_DISCONNECT:
                log::debug(central::COMPONENT,
                    "GAP event: BLE_GAP_EVENT_DISCONNECT.");

                cent->m_dispatcher.async_dispatch<events::on_disconnect>(
                    event->disconnect.conn.conn_handle);

                return 0;
            case BLE_GAP_EVENT_NOTIFY_RX:
                log::debug(central::COMPONENT,
                    "GAP event: BLE_GAP_EVENT_NOTIFY_RX.");

                if (event->notify_rx.indication == 1)
                {
                    return 0;
                }

                // attr->om_pkthdr_len != 0 checks if buffer is a normal buffer
                if (event->notify_rx.om->om_pkthdr_len != 0)
                {
                    cent->m_notify_buffer.resize(event->notify_rx.om->om_len);

                    ::os_mbuf_copydata(event->notify_rx.om,
                        0,
                        cent->m_notify_buffer.size(),
                        static_cast<void*>(cent->m_notify_buffer.data()));

                    log::verbose(central::COMPONENT,
                        "Got data: {}.",
                        cent->m_notify_buffer);

                    cent->m_dispatcher.async_dispatch<events::notify_rx>(
                        events::notify_rx_args{
                            event->notify_rx.conn_handle,
                            event->notify_rx.attr_handle,
                            std::move(cent->m_notify_buffer),
                        });

                    cent->m_notify_buffer = {};
                }

                return 0;
            default:
                log::info(central::COMPONENT, "GAP event: {}.", event->type);
                return 0;
            }
        }
    } // namespace impl

    central::central(event::context& ctx) noexcept :
        m_dispatcher{ ctx },
        m_receiver{ m_dispatcher.make_receiver() },
        m_notify_buffer{}
    {
    }

    central::central(central&& other) noexcept :
        m_dispatcher{ std::move(other.m_dispatcher) },
        m_receiver{ m_dispatcher.make_receiver() },
        m_notify_buffer{ std::move(other.m_notify_buffer) }
    {
    }

    central& central::operator=(central&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_dispatcher    = std::move(other.m_dispatcher);
        m_receiver      = m_dispatcher.make_receiver();
        m_notify_buffer = std::move(other.m_notify_buffer);

        return *this;
    }
} // namespace b2h::ble::gap
