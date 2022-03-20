#include "xiaomi/lywsd03mmc.hpp"

namespace b2h::device::xiaomi
{
    lywsd03mmc::lywsd03mmc(std::unique_ptr<mqtt::client>&& mqtt_client,
        std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept :
        base{ std::move(mqtt_client), std::move(gatt_client) },
        m_state{
            this->gatt_client(),
            this->mqtt_client(),
            0U,
            0.0f,
            0,
            make_process_external_event(),
        },
        m_fsm{ m_state }
    {
    }

    void lywsd03mmc::on_connected() noexcept
    {
        m_fsm.process_event(lywsd03mmc_impl::events::connected{});
    }

    void lywsd03mmc::on_notify(std::uint16_t attribute_handle,
        std::vector<std::uint8_t>&& data) noexcept
    {
        m_fsm.process_event(lywsd03mmc_impl::events::notify{
            attribute_handle,
            std::move(data),
        });
    }
} // namespace b2h::device::xiaomi
