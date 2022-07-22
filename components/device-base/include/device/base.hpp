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

#ifndef B2H_DEVICE_BASE_HPP
#define B2H_DEVICE_BASE_HPP

#include <functional>
#include <memory>
#include <utility>

#include "ble/gap/central.hpp"
#include "ble/gatt/client.hpp"
#include "hass/device_types.hpp"
#include "mqtt/client.hpp"
#include "tl/expected.hpp"
#include "utils/mac.hpp"

namespace b2h::device
{
    class base
    {
    public:
        base()            = delete;
        base(const base&) = delete;
        base(base&&)      = default;
        virtual ~base()   = default;

        base(mqtt::client mqtt_client, ble::gatt::client gatt_client) noexcept;

        base& operator=(const base&) = delete;
        base& operator=(base&&) = default;

        virtual void on_connected();
        virtual void on_disconnected();
        virtual void on_notify(const std::uint16_t attribute_handle,
            std::vector<std::uint8_t>&& data);
        std::uint16_t connection_handle() const noexcept;

        mqtt::client& mqtt_client() noexcept;
        ble::gatt::client& gatt_client() noexcept;

    private:
        mqtt::client m_mqtt_client;
        ble::gatt::client m_gatt_client;
    };

} // namespace b2h::device

#endif
