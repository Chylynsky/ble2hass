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

#ifndef B2H_DEVICE_BUILDER_HPP
#define B2H_DEVICE_BUILDER_HPP

#include "ble/gap/central.hpp"
#include "device/base.hpp"
#include "device/factory.hpp"

#include <future>
#include <memory>
#include <mutex>

namespace b2h::device
{
    namespace impl
    {
        class device_builder
        {
        public:
            device_builder(mqtt::client mqtt_client,
                ble::gatt::client gatt_client) noexcept;

            device_builder()                      = delete;
            device_builder(const device_builder&) = delete;
            device_builder(device_builder&&)      = default;

            device_builder& operator=(const device_builder&) = delete;
            device_builder& operator=(device_builder&&) = default;

            device_variant_t build(const std::string_view name) &&;

        private:
            mqtt::client m_mqtt_client;
            ble::gatt::client m_gatt_client;
        };

        class gatt_builder
        {
        public:
            explicit gatt_builder(
                event::context& context, mqtt::client mqtt_client) noexcept :
                m_context{ context },
                m_mqtt_client{ std::move(mqtt_client) }
            {
            }

            gatt_builder()                    = delete;
            gatt_builder(const gatt_builder&) = delete;
            gatt_builder(gatt_builder&&)      = default;

            gatt_builder& operator=(const gatt_builder&) = delete;
            gatt_builder& operator=(gatt_builder&&) = default;

            device_builder gatt_client(
                ble::gap::central& central, const utils::mac& mac) &&
            {
                using namespace std::literals;
                using connect_arg_t = events::ble::gap::connect::expected_type;

                std::packaged_task<connect_arg_t(connect_arg_t &&)>
                    connection_task{
                        [](auto&& arg) { return arg; },
                    };

                auto connection_future = connection_task.get_future();
                central.async_connect(mac, 15s, std::ref(connection_task));
                auto result = connection_future.get();

                if (!result.has_value())
                {
                    throw std::runtime_error(
                        "Failed to initialize GATT client.");
                }

                return device_builder{
                    std::move(m_mqtt_client),
                    ble::gatt::client{
                        m_context,
                        result.value().connection_handle,
                        mac,
                    },
                };
            }

        private:
            event::context& m_context;
            mqtt::client m_mqtt_client;
        };

        class mqtt_builder
        {
        public:
            explicit mqtt_builder(event::context& context) noexcept :
                m_context{ context }
            {
            }

            mqtt_builder()                    = delete;
            mqtt_builder(const mqtt_builder&) = delete;
            mqtt_builder(mqtt_builder&&)      = default;

            mqtt_builder& operator=(const mqtt_builder&) = delete;
            mqtt_builder& operator=(mqtt_builder&&) = default;

            gatt_builder mqtt_client(const mqtt::config& config) &&
            {
                using connect_arg_t = events::mqtt::connect::expected_type;

                mqtt::client client{ m_context };

                client.config(config);

                std::packaged_task<connect_arg_t(connect_arg_t &&)>
                    connection_task{
                        [](auto&& arg) { return arg; },
                    };

                auto connection_future = connection_task.get_future();
                client.async_connect(std::ref(connection_task));
                auto result = connection_future.get();

                if (!result.has_value())
                {
                    throw std::runtime_error(
                        "Failed to initialize MQTT client.");
                }

                return gatt_builder{
                    m_context,
                    std::move(client),
                };
            }

        private:
            event::context& m_context;
        };
    } // namespace impl

    class builder
    {
    public:
        builder()               = default;
        builder(const builder&) = delete;
        builder(builder&&)      = default;

        builder& operator=(const builder&) = delete;
        builder& operator=(builder&&) = default;

        impl::mqtt_builder context(event::context& context) noexcept
        {
            return impl::mqtt_builder{
                context,
            };
        }
    };

} // namespace b2h::device

#endif
