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

#ifndef B2H_WIFI_STATION_HPP
#define B2H_WIFI_STATION_HPP

#include <memory>
#include <string_view>

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "tl/expected.hpp"

#include "event/event.hpp"
#include "utils/esp_exception.hpp"
#include "utils/logger.hpp"

namespace b2h
{
    namespace events::wifi
    {
        struct start : public event::basic_event<void, esp_err_t> {
        };

        struct stop : public event::basic_event<void, esp_err_t> {
        };

        struct connected : public event::basic_event<void, esp_err_t> {
        };

        struct disconnected : public event::basic_event<void, esp_err_t> {
        };
    } // namespace events::wifi

    namespace wifi
    {
        namespace impl
        {
            void event_handler(void* arg, esp_event_base_t event_base,
                int32_t event_id, void* event_data) noexcept;
        }

        struct station_config {
            std::string_view ssid;
            std::string_view password;
        };

        class station
        {
        public:
            static constexpr auto WIFI_MODE = WIFI_MODE_STA;
            static constexpr auto WIFI_INTERFACE =
                static_cast<wifi_interface_t>(ESP_IF_WIFI_STA);

            using dispatcher_type =
                event::dispatcher<events::wifi::start, events::wifi::stop,
                    events::wifi::connected, events::wifi::disconnected>;
            using receiver_type = dispatcher_type::receiver_type;

            station(event::context& context);

            station(const station&) = delete;

            station(station&& other) = delete;

            ~station();

            station& operator=(const station&) = delete;

            station& operator=(station&& other) = delete;

            void config(const station_config& config) noexcept;

            station_config config() const noexcept;

            template<typename HandlerT>
            void async_start(HandlerT&& handler) noexcept
            {
                esp_err_t result = ESP_OK;

                m_receiver.async_receive<events::wifi::start>(
                    std::forward<HandlerT>(handler));

                if (result = esp_wifi_start(); result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "WiFi start failed with error code {} [{}].",
                        result,
                        esp_err_to_name(result));
                    m_dispatcher.async_dispatch<events::wifi::start>(
                        tl::make_unexpected(result));
                }
            }

            template<typename HandlerT>
            void async_connect(HandlerT&& handler) noexcept
            {
                esp_err_t result = ESP_OK;

                m_receiver.async_receive<events::wifi::connected>(
                    std::forward<HandlerT>(handler));

                if (result = esp_wifi_connect(); result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "WiFi connect failed with error code {} [{}].",
                        result,
                        esp_err_to_name(result));
                    m_dispatcher.async_dispatch<events::wifi::connected>(
                        tl::make_unexpected(result));
                }
            }

            template<typename HandlerT>
            void on_disconnect(HandlerT&& handler) noexcept
            {
                m_receiver.async_receive<events::wifi::disconnected>(
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_disconnect(HandlerT&& handler) noexcept
            {
                esp_err_t result = ESP_OK;

                m_receiver.async_receive<events::wifi::disconnected>(
                    std::forward<HandlerT>(handler));

                if (result = esp_wifi_disconnect(); result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "WiFi disconnect failed with error code {} [{}].",
                        result,
                        esp_err_to_name(result));
                    m_dispatcher.async_dispatch<events::wifi::disconnected>(
                        tl::make_unexpected(result));
                }
            }

            template<typename HandlerT>
            void async_stop(HandlerT&& handler) noexcept
            {
                esp_err_t result = ESP_OK;

                m_receiver.async_receive<events::wifi::stop>(
                    std::forward<HandlerT>(handler));

                if (result = esp_wifi_stop(); result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "WiFi stop failed with error code {} [{}].",
                        result,
                        esp_err_to_name(result));
                    m_dispatcher.async_dispatch<events::wifi::stop>(
                        tl::make_unexpected(result));
                }
            }

        private:
            static constexpr std::string_view COMPONENT{ "wifi::station" };

            friend void impl::event_handler(void* arg,
                esp_event_base_t event_base, int32_t event_id,
                void* event_data) noexcept;

            esp_netif_t* m_netif;
            wifi_config_t m_config;
            dispatcher_type m_dispatcher;
            receiver_type m_receiver;
        };
    } // namespace wifi
} // namespace b2h

#endif
