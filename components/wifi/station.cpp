#include "wifi/station.hpp"

namespace b2h::wifi
{
    namespace impl
    {
        void event_handler(void* arg, esp_event_base_t event_base,
            int32_t event_id, void* event_data) noexcept
        {
            auto dispatcher = static_cast<station::dispatcher_type*>(arg);

            if (event_base != WIFI_EVENT)
            {
                return;
            }

            if (event_id == WIFI_EVENT_STA_START)
            {
                log::debug(station::COMPONENT,
                    "WiFi event: WIFI_EVENT_STA_START.");
                dispatcher->async_dispatch<events::wifi::start>({});
            }
            else if (event_id == WIFI_EVENT_STA_STOP)
            {
                log::debug(station::COMPONENT,
                    "WiFi event: WIFI_EVENT_STA_STOP.");
                dispatcher->async_dispatch<events::wifi::stop>({});
            }
            else if (event_id == WIFI_EVENT_STA_CONNECTED)
            {
                log::debug(station::COMPONENT,
                    "WiFi event: WIFI_EVENT_STA_CONNECTED.");
                dispatcher->async_dispatch<events::wifi::connected>({});
            }
            else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
            {
                log::debug(station::COMPONENT,
                    "WiFi event: WIFI_EVENT_STA_DISCONNECTED.");
                log::warning(station::COMPONENT, "WiFi disconnected.");

                dispatcher->async_dispatch<events::wifi::connected>(
                    tl::make_unexpected(ESP_ERR_WIFI_NOT_CONNECT));
                dispatcher->async_dispatch<events::wifi::disconnected>({});
            }
        }

    } // namespace impl

    station::station(event::context& context) :
        m_netif{ nullptr },
        m_config{},
        m_dispatcher{ context },
        m_receiver{ m_dispatcher.make_receiver() }
    {
        const wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

        esp_err_t result = ESP_OK;

        if (result = esp_netif_init(); result != ESP_OK)
        {
            log::error(COMPONENT,
                "NETIF initialization failed. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_throw;
        }

        if (m_netif = esp_netif_create_default_wifi_sta(); !m_netif)
        {
            log::error(COMPONENT, "WiFi default station creation failed.");
            goto err_netif_deinit;
        }

        if (result = esp_wifi_init(&wifi_init_config); result != ESP_OK)
        {
            log::error(COMPONENT,
                "WiFi initialization failed. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_destroy_wifi_sta;
        }

        if (result = esp_event_handler_instance_register(WIFI_EVENT,
                WIFI_EVENT_STA_START,
                &impl::event_handler,
                &m_dispatcher,
                nullptr);
            result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to register WIFI_EVENT_STA_START event. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_wifi_deinit;
        }

        if (result = esp_event_handler_instance_register(WIFI_EVENT,
                WIFI_EVENT_STA_STOP,
                &impl::event_handler,
                &m_dispatcher,
                nullptr);
            result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to register WIFI_EVENT_STA_STOP event. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_unregister_sta_start_event;
        }

        if (result = esp_event_handler_instance_register(WIFI_EVENT,
                WIFI_EVENT_STA_CONNECTED,
                &impl::event_handler,
                &m_dispatcher,
                nullptr);
            result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to register WIFI_EVENT_STA_CONNECTED event. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_unregister_sta_stop_event;
        }

        if (result = esp_event_handler_instance_register(WIFI_EVENT,
                WIFI_EVENT_STA_DISCONNECTED,
                &impl::event_handler,
                &m_dispatcher,
                nullptr);
            result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to register WIFI_EVENT_STA_DISCONNECTED event. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_unregister_sta_connected_event;
        }

        if (result = esp_wifi_set_mode(WIFI_MODE); result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to set WiFi mode. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
            goto err_unregister_disconnected_event;
        }

        log::info(COMPONENT, "WiFi station initialized successfully.");
        return;

err_unregister_disconnected_event:
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_DISCONNECTED,
            nullptr);
err_unregister_sta_connected_event:
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_CONNECTED,
            nullptr);
err_unregister_sta_stop_event:
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_STOP,
            nullptr);
err_unregister_sta_start_event:
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_START,
            nullptr);
err_wifi_deinit:
        esp_wifi_deinit();
err_destroy_wifi_sta:
        esp_netif_destroy_default_wifi(m_netif);
        m_netif = nullptr;
err_netif_deinit:
        esp_netif_deinit();
err_throw:
        throw utils::esp_exception{ "WiFi station initialization failed.",
            result };
    }

    station::station(station&& other) noexcept :
        m_netif{ other.m_netif },
        m_config{ std::move(other.m_config) },
        m_dispatcher{ std::move(other.m_dispatcher) },
        m_receiver{ m_dispatcher.make_receiver() }
    {
        other.m_netif = nullptr;
    }

    station::~station()
    {
        esp_wifi_stop();
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_DISCONNECTED,
            nullptr);
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_CONNECTED,
            nullptr);
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_STOP,
            nullptr);
        esp_event_handler_instance_unregister(WIFI_EVENT,
            WIFI_EVENT_STA_START,
            nullptr);
        esp_wifi_deinit();
        esp_netif_destroy_default_wifi(m_netif);
        esp_netif_deinit();

        m_netif = nullptr;
    }

    station& station::operator=(station&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_netif       = other.m_netif;
        other.m_netif = nullptr;
        m_config      = std::move(other.m_config);
        m_dispatcher  = std::move(other.m_dispatcher);
        m_receiver    = m_dispatcher.make_receiver();

        return *this;
    }

    void station::config(const station_config& config) noexcept
    {
        esp_err_t result = ESP_OK;

        m_config = {};

        std::copy(config.ssid.cbegin(),
            config.ssid.cend(),
            reinterpret_cast<char*>(&m_config.sta.ssid[0]));
        std::copy(config.password.cbegin(),
            config.password.cend(),
            reinterpret_cast<char*>(&m_config.sta.password[0]));

        if (result = esp_wifi_set_config(WIFI_INTERFACE, &m_config);
            result != ESP_OK)
        {
            log::error(COMPONENT,
                "Failed to set WiFi configuration. Error code: {} [{}].",
                result,
                esp_err_to_name(result));
        }
    }

    station_config station::config() const noexcept
    {
        return station_config{ { reinterpret_cast<const char*>(
                                   &m_config.sta.ssid[0]) },
            { reinterpret_cast<const char*>(&m_config.sta.password[0]) } };
    }

} // namespace b2h::wifi
