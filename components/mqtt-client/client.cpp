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

#include "mqtt/client.hpp"

namespace b2h::mqtt
{
    std::uint8_t client::id = 0;

    client::client(event::context& ctx) noexcept :
        id_buff{},
        m_dispatcher{ ctx },
        m_receiver{ m_dispatcher.make_receiver() },
        m_handle{ nullptr, &::esp_mqtt_client_destroy },
        m_buffer{}
    {
    }

    tl::expected<void, esp_err_t> client::config(
        const mqtt::config& cfg) noexcept
    {
        log::debug(COMPONENT, "Got MQTT config. URI: {}", cfg.uri);

        esp_mqtt_client_config_t config{};
        config.uri = cfg.uri.c_str();

        if (!cfg.username.empty())
        {
            config.username = cfg.username.c_str();
        }

        if (!cfg.password.empty())
        {
            config.password = cfg.password.c_str();
        }

        fmt::format_to(id_buff.begin(), "{0:2x}", id);
        ++id;

        config.client_id = id_buff.data();
        config.disable_clean_session =
            static_cast<int>(cfg.disable_clean_session);

        log::debug(COMPONENT, "Client ID set to: {}.", id_buff.data());

        if (m_handle =
                handle_ptr{
                    ::esp_mqtt_client_init(&config),
                    &::esp_mqtt_client_destroy,
                };
            !m_handle)
        {
            log::error(COMPONENT,
                "Failed to initialize MQTT client, error code: {0} [{1}]",
                ESP_FAIL,
                ::esp_err_to_name(ESP_FAIL));
            return tl::make_unexpected(ESP_FAIL);
        }

        return {};
    }

    void client::mqtt_event_callback(void* handler_args, esp_event_base_t base,
        int32_t event_id, void* event) noexcept
    {
        using namespace b2h::events::mqtt;

        esp_mqtt_event_handle_t event_data =
            reinterpret_cast<esp_mqtt_event_handle_t>(event);
        client* client_ptr = static_cast<client*>(handler_args);

        switch (event_data->event_id)
        {
        case MQTT_EVENT_DATA:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_DATA");
            log::verbose(COMPONENT,
                "topic: {}, data: {}",
                std::string_view{ event_data->topic,
                    static_cast<std::size_t>(event_data->topic_len) },
                std::string_view{ event_data->data,
                    static_cast<std::size_t>(event_data->data_len) });

            client_ptr->m_buffer.resize(
                event_data->topic_len + event_data->data_len);

            {
                auto iter = std::copy(event_data->topic,
                    event_data->topic + event_data->topic_len,
                    client_ptr->m_buffer.begin());

                std::copy(event_data->data,
                    event_data->data + event_data->data_len,
                    iter);
            }

            client_ptr->m_dispatcher.async_dispatch<data>(data_args{
                std::string_view{
                    client_ptr->m_buffer.data(),
                    static_cast<std::size_t>(event_data->topic_len),
                },
                std::string_view{
                    client_ptr->m_buffer.data() + event_data->topic_len,
                    static_cast<std::size_t>(event_data->data_len),
                },
            });

            break;
        }
        case MQTT_EVENT_PUBLISHED:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_PUBLISHED");
            client_ptr->m_dispatcher.async_dispatch<publish>({});
            break;
        }
        case MQTT_EVENT_SUBSCRIBED:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_SUBSCRIBED");
            client_ptr->m_dispatcher.async_dispatch<subscribe>({});
            break;
        }
        case MQTT_EVENT_UNSUBSCRIBED:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_UNSUBSCRIBED");
            client_ptr->m_dispatcher.async_dispatch<unsubscribe>({});
            break;
        }
        case MQTT_EVENT_CONNECTED:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_CONNECTED");
            client_ptr->m_dispatcher.async_dispatch<connect>({});
            // client_ptr->m_dispatcher.async_dispatch<connected>({});
            break;
        }
        case MQTT_EVENT_DISCONNECTED:
        {
            log::debug(COMPONENT, "Event: MQTT_EVENT_DISCONNECTED");
            client_ptr->m_dispatcher.async_dispatch<disconnect>({});
            // client_ptr->m_dispatcher.async_dispatch<disconnected>({});
            break;
        }
        default:
        {
            log::debug(COMPONENT, "Other event: {0}.", event_data->event_id);
            break;
        }
        }
    }
} // namespace b2h::mqtt
