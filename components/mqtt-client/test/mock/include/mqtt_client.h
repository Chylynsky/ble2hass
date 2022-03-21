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

#include "esp_err.h"

#include <algorithm>
#include <cstdint>
#include <future>

inline constexpr const char* TEST_DATA = "TEST";

enum esp_mqtt_event_id_t : int
{
    ESP_EVENT_ANY_ID = -1,
    MQTT_EVENT_BEFORE_CONNECT,
    MQTT_EVENT_CONNECTED,
    MQTT_EVENT_DISCONNECTED,
    MQTT_EVENT_SUBSCRIBED,
    MQTT_EVENT_UNSUBSCRIBED,
    MQTT_EVENT_PUBLISHED,
    MQTT_EVENT_DATA,
    MQTT_EVENT_ERROR,
};

struct esp_mqtt_client_config_t {
    const char* uri;
    const char* username;
    const char* password;
};

using esp_event_base_t = const char*;

using esp_event_handler_t = void (*)(void* event_handler_arg,
    esp_event_base_t event_base, std::int32_t event_id, void* event_data);

struct esp_mqtt_event_t {
    esp_mqtt_event_id_t event_id;
    const char* data;
    const char* topic;
    std::size_t topic_len;
    std::size_t data_len;
};

using esp_mqtt_event_handle_t = esp_mqtt_event_t*;

struct esp_mqtt_client {
    esp_mqtt_client_config_t config;
    esp_event_handler_t event_handler;
    void* event_handler_arg;
    std::vector<esp_mqtt_event_id_t> events;
    std::future<void> dispatch_fut;

    void dispatch(esp_mqtt_event_id_t event_id)
    {
        esp_mqtt_event_t event;
        if (std::find_if(events.cbegin(), events.cend(), [event_id](auto val) {
                return val == ESP_EVENT_ANY_ID || val == event_id;
            }) != events.cend())
        {
            event.event_id = event_id;
            event_handler(event_handler_arg, nullptr, event_id, &event);
        }
    }

    void publish(std::string topic, std::string data)
    {
        esp_mqtt_event_t event;
        event.event_id  = MQTT_EVENT_DATA;
        event.data      = data.data();
        event.data_len  = data.size();
        event.topic     = topic.data();
        event.topic_len = topic.size();
        event_handler(event_handler_arg, nullptr, event.event_id, &event);
    }
};

using esp_mqtt_client_handle_t = esp_mqtt_client*;

inline esp_err_t esp_mqtt_set_config(esp_mqtt_client_handle_t client,
    const esp_mqtt_client_config_t* config) noexcept
{
    return ESP_OK;
}

inline esp_err_t esp_mqtt_client_start(esp_mqtt_client_handle_t client) noexcept
{
    client->dispatch_fut = std::async(std::launch::async,
        [client]() { client->dispatch(MQTT_EVENT_CONNECTED); });
    return ESP_OK;
}

inline esp_err_t esp_mqtt_client_stop(esp_mqtt_client_handle_t client) noexcept
{
    client->dispatch_fut = std::async(std::launch::async,
        [client]() { client->dispatch(MQTT_EVENT_DISCONNECTED); });
    return ESP_OK;
}

inline esp_err_t esp_mqtt_client_register_event(esp_mqtt_client_handle_t client,
    esp_mqtt_event_id_t event, esp_event_handler_t event_handler,
    void* event_handler_arg) noexcept
{
    client->events.push_back(event);
    client->event_handler     = event_handler;
    client->event_handler_arg = event_handler_arg;
    return ESP_OK;
}

inline int esp_mqtt_client_publish(esp_mqtt_client_handle_t client,
    const char* topic, const char* data, int len, int qos, int retain) noexcept
{
    client->dispatch_fut = std::async(std::launch::async,
        [client]() { client->dispatch(MQTT_EVENT_PUBLISHED); });
    return 23;
}

inline int esp_mqtt_client_subscribe(
    esp_mqtt_client_handle_t client, const char* topic, int qos) noexcept
{
    client->dispatch_fut =
        std::async(std::launch::async, [client, topic{ std::string(topic) }]() {
            client->dispatch(MQTT_EVENT_SUBSCRIBED);
            client->publish(std::move(topic), TEST_DATA);
        });
    return 23;
}

inline int esp_mqtt_client_unsubscribe(
    esp_mqtt_client_handle_t client, const char* topic) noexcept
{
    client->dispatch_fut = std::async(std::launch::async,
        [client]() { client->dispatch(MQTT_EVENT_UNSUBSCRIBED); });
    return 23;
}

inline esp_mqtt_client_handle_t esp_mqtt_client_init(
    const esp_mqtt_client_config_t*)
{
    return new esp_mqtt_client{};
}

inline esp_err_t esp_mqtt_client_destroy(
    esp_mqtt_client_handle_t client) noexcept
{
    delete client;
    return ESP_OK;
}
