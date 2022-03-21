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
