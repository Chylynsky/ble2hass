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

#ifndef B2H_MQTT_CLIENT_HPP
#define B2H_MQTT_CLIENT_HPP

#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "esp_err.h"
#include "mqtt_client.h"

#include "tl/expected.hpp"

#include "event/event.hpp"
#include "utils/logger.hpp"

namespace b2h
{
    namespace events::mqtt
    {
        struct data_args {
            std::string_view topic;
            std::string_view data;
        };

        struct data : public event::basic_event<data_args, esp_err_t> {
        };

        struct subscribe : public event::basic_event<void, esp_err_t> {
        };

        struct unsubscribe : public event::basic_event<void, esp_err_t> {
        };

        struct publish : public event::basic_event<void, esp_err_t> {
        };

        struct connect : public event::basic_event<void, esp_err_t> {
        };

        struct disconnect : public event::basic_event<void, esp_err_t> {
        };

        struct connected : public event::basic_event<void, esp_err_t> {
        };

        struct disconnected : public event::basic_event<void, esp_err_t> {
        };
    } // namespace events::mqtt

    namespace mqtt
    {
        struct config {
            std::string uri;
            std::string username;
            std::string password;
        };

        static constexpr std::string_view COMPONENT = "mqtt::client";

        class client final
        {
        public:
            client() = delete;

            explicit client(event::context& ctx) noexcept;

            client(const client&) = delete;

            client(client&& other) = delete;

            client& operator=(const client&) = delete;

            client& operator=(client&& other) = delete;

            ~client() = default;

            auto& receiver() noexcept
            {
                return m_receiver;
            }

            tl::expected<void, esp_err_t> config(const config& cfg) noexcept;

            template<typename HandlerT>
            void async_connect(HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                assert(static_cast<bool>(m_handle));

                log::debug(COMPONENT, "Attempting to connect.");

                m_receiver.async_receive<connect>(
                    std::forward<HandlerT>(handler));

                esp_err_t result = ESP_OK;

                if (result = ::esp_mqtt_client_register_event(m_handle.get(),
                        static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID),
                        &mqtt_event_callback,
                        static_cast<void*>(this));
                    result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "Failed to register events, error code: {0} [{1}]",
                        result,
                        ::esp_err_to_name(result));
                    m_dispatcher.async_dispatch<connect>(
                        tl::make_unexpected(result));
                    return;
                }

                if (result = ::esp_mqtt_client_start(m_handle.get());
                    result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "Failed to start, error code: {0} [{1}]",
                        result,
                        ::esp_err_to_name(result));
                    m_dispatcher.async_dispatch<connect>(
                        tl::make_unexpected(result));
                    return;
                }

                log::info(COMPONENT, "Successfully started MQTT client.");
            }

            template<typename HandlerT>
            void async_disconnect(HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                assert(static_cast<bool>(m_handle));

                log::debug(COMPONENT, "Attempting to disconnect.");

                m_receiver.async_receive<disconnect>(
                    std::forward<HandlerT>(handler));

                if (::esp_mqtt_client_stop(m_handle.get()) != ESP_OK)
                {
                    m_dispatcher.async_dispatch<events::mqtt::disconnect>(
                        tl::make_unexpected(ESP_FAIL));
                }
            }

            template<typename HandlerT>
            void async_subscribe(
                const char* topic, int qos, HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                assert(static_cast<bool>(m_handle));

                log::debug(COMPONENT, "Subscribing to topic.");
                log::verbose(COMPONENT, "topic: {}", topic);

                m_receiver.async_receive<subscribe>(
                    std::forward<HandlerT>(handler));

                if (::esp_mqtt_client_subscribe(m_handle.get(), topic, qos) ==
                    -1)
                {
                    m_dispatcher.async_dispatch<subscribe>(
                        tl::make_unexpected(ESP_FAIL));
                }
            }

            template<typename HandlerT>
            void async_subscribe(
                const std::string& topic, int qos, HandlerT&& handler) noexcept
            {
                async_subscribe(topic.c_str(),
                    qos,
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_subscribe(const char* topic, HandlerT&& handler) noexcept
            {
                async_subscribe(topic, 0, std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_subscribe(
                const std::string& topic, HandlerT&& handler) noexcept
            {
                async_subscribe(topic.c_str(),
                    0,
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_unsubscribe(
                const char* topic, HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                assert(static_cast<bool>(m_handle));

                log::debug(COMPONENT, "Unsubscribing from topic.");
                log::verbose(COMPONENT, "topic: {}", topic);

                m_receiver.async_receive<unsubscribe>(
                    std::forward<HandlerT>(handler));

                if (::esp_mqtt_client_unsubscribe(m_handle.get(), topic) == -1)
                {
                    m_dispatcher.async_dispatch<unsubscribe>(
                        tl::make_unexpected(ESP_FAIL));
                }
            }

            template<typename HandlerT>
            void async_unsubscribe(
                const std::string& topic, HandlerT&& handler) noexcept
            {
                async_unsubscribe(topic.c_str(),
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_publish(const char* topic, const std::string_view data,
                int qos, bool retain, HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                assert(static_cast<bool>(m_handle));

                log::debug(COMPONENT, "Publishing MQTT data.");
                log::verbose(COMPONENT, "topic: {}; data: {}", topic, data);

                m_receiver.async_receive<publish>(
                    std::forward<HandlerT>(handler));

                if (::esp_mqtt_client_enqueue(m_handle.get(),
                        topic,
                        data.data(),
                        data.size(),
                        qos,
                        static_cast<int>(retain),
                        true) == -1)
                {
                    m_dispatcher.async_dispatch<publish>(
                        tl::make_unexpected(ESP_FAIL));
                    return;
                }

                if (qos == 0)
                {
                    m_dispatcher.async_dispatch<publish>({});
                }
            }

            template<typename HandlerT>
            void async_publish(const std::string& topic, std::string_view data,
                int qos, bool retain, HandlerT&& handler) noexcept
            {
                async_publish(topic.c_str(),
                    data,
                    qos,
                    retain,
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void async_receive(HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;

                log::debug(COMPONENT, "Waiting for data.");
                m_receiver.async_receive<data>(std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void on_connect(HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;
                m_receiver.async_receive<connected>(
                    std::forward<HandlerT>(handler));
            }

            template<typename HandlerT>
            void on_disconnect(HandlerT&& handler) noexcept
            {
                using namespace b2h::events::mqtt;
                m_receiver.async_receive<disconnected>(
                    std::forward<HandlerT>(handler));
            }

        private:
            // clang-format off
            using dispatcher_type = event::dispatcher<
                b2h::events::mqtt::connect,
                b2h::events::mqtt::data,
                b2h::events::mqtt::disconnect,
                b2h::events::mqtt::publish,
                b2h::events::mqtt::subscribe,
                b2h::events::mqtt::unsubscribe,
                b2h::events::mqtt::connected,
                b2h::events::mqtt::disconnected
            >;

            using receiver_type = typename dispatcher_type::receiver_type;

            using handle_ptr = std::unique_ptr<
                esp_mqtt_client, 
                decltype(&::esp_mqtt_client_destroy)
            >;
            // clang-format on

            static std::uint8_t id;

            std::array<char, 3> id_buff;

            dispatcher_type m_dispatcher;
            receiver_type m_receiver;

            handle_ptr m_handle;

            std::string m_buffer;

            static void mqtt_event_callback(void* handler_args,
                esp_event_base_t base, std::int32_t event_id,
                void* event) noexcept;
        };

    } // namespace mqtt
} // namespace b2h

#endif
