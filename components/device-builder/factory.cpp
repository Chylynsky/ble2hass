#include "device/factory.hpp"

// Include device implementation headers

#include "xiaomi/lywsd03mmc.hpp"
#include "xiaomi/mikettle.hpp"

namespace b2h::device
{
    template<typename DeviceT>
    constexpr auto make_factory() noexcept
    {
        return [](std::unique_ptr<mqtt::client>&& mqtt_client,
                   std::unique_ptr<ble::gatt::client>&& gatt_client)
                   -> std::shared_ptr<interface> {
            return std::make_shared<DeviceT>(std::move(mqtt_client),
                std::move(gatt_client));
        };
    }

    // Add devices as: { "device name", make_factory<device_type>() }

    const std::map<std::string_view, factory_function_t> g_factory_map{
        {
            "MiKettle",
            make_factory<xiaomi::mikettle>(),
        },
        {
            "Mi Temperature & Humidity Monitor 2",
            make_factory<xiaomi::lywsd03mmc>(),
        },
    };
} // namespace b2h::device
