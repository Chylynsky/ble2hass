#include "xiaomi/mikettle.hpp"
#include "utils/logger.hpp"

namespace b2h::device::xiaomi
{
    mikettle::mikettle(std::unique_ptr<mqtt::client>&& mqtt_client,
        std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept :
        base{ std::move(mqtt_client), std::move(gatt_client) },
        m_state{
            this->gatt_client(),
            this->mqtt_client(),
            {},
            make_process_external_event(),
        },
        m_fsm{ m_state }
    {
    }

    void mikettle::on_connected() noexcept
    {
        m_fsm.process_event(mikettle_impl::events::connected{});
    }

    void mikettle::on_notify(std::uint16_t attribute_handle,
        std::vector<std::uint8_t>&& data) noexcept
    {
        m_fsm.process_event(
            mikettle_impl::events::notify{ attribute_handle, std::move(data) });
    }
} // namespace b2h::device::xiaomi
