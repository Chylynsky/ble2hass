#include "device/builder.hpp"

#include "device/factory.hpp"

namespace b2h::device
{
    namespace impl
    {
        device_builder::device_builder(
            std::unique_ptr<mqtt::client>&& mqtt_client,
            std::unique_ptr<ble::gatt::client>&& gatt_client) noexcept :
            m_mqtt_client{ std::move(mqtt_client) },
            m_gatt_client{ std::move(gatt_client) }
        {
        }

        std::shared_ptr<interface> device_builder::build(
            const std::string_view name) && noexcept
        {
            if (auto iter = g_factory_map.find(name);
                iter != g_factory_map.end())
            {
                auto& factory_fun = iter->second;
                auto device       = factory_fun(std::move(m_mqtt_client),
                    std::move(m_gatt_client));

                device->on_connected();
                return device;
            }

            return {};
        }
    } // namespace impl
} // namespace b2h::device
