#include "device/base.hpp"

namespace b2h::device
{
    base::base(std::unique_ptr<mqtt::client>&& mqtt_client,
        std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept :
        m_mqtt_client{ std::move(mqtt_client) },
        m_gatt_client{ std::move(gatt_client) }
    {
    }

    void base::on_connected() noexcept { }

    void base::on_disconnected() noexcept { }

    void base::on_notify(std::uint16_t attribute_handle,
        std::vector<std::uint8_t>&& data) noexcept
    {
    }

    std::uint16_t base::connection_handle() noexcept
    {
        return m_gatt_client->connection_handle();
    }

    mqtt::client& base::mqtt_client() noexcept
    {
        return *m_mqtt_client;
    }

    ble::gatt::client& base::gatt_client() noexcept
    {
        return *m_gatt_client;
    }
} // namespace b2h::device
