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

#include "device/builder.hpp"

namespace b2h::device
{
    namespace impl
    {
        device_builder::device_builder(
            mqtt::client mqtt_client, ble::gatt::client gatt_client) noexcept :
            m_mqtt_client{ std::move(mqtt_client) },
            m_gatt_client{ std::move(gatt_client) }
        {
        }

        device_variant_t device_builder::build(const std::string_view name) &&
        {
            if (auto iter = g_factory_map.find(name);
                iter != g_factory_map.end())
            {
                auto& factory_fun = iter->second;
                return factory_fun(std::move(m_mqtt_client),
                    std::move(m_gatt_client));
            }

            throw std::invalid_argument{ "Invalid device type." };
        }
    } // namespace impl
} // namespace b2h::device
