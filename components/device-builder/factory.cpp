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
