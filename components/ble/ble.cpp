#include "ble/ble.hpp"

namespace b2h::ble
{
    namespace impl
    {
        std::function<void(void)> g_on_sync{};

        void on_sync() noexcept
        {
            log::debug(context::COMPONENT, "NimBLE host and controller sync.");
            // assert(!ble_hs_util_ensure_addr(0));
            g_on_sync();
            ble_hs_cfg.sync_cb = nullptr;
            g_on_sync          = nullptr;
        }

    } // namespace impl

    context::context(event::context& ctx) noexcept :
        m_dispatcher{ ctx },
        m_receiver{ m_dispatcher.make_receiver() }
    {
    }

    context::context(context&& other) noexcept :
        m_dispatcher{ std::move(other.m_dispatcher) },
        m_receiver{ m_dispatcher.make_receiver() }
    {
    }

    context& context::operator=(context&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_dispatcher = std::move(other.m_dispatcher);
        m_receiver   = m_dispatcher.make_receiver();

        return *this;
    }

    tl::expected<void, esp_err_t> context::deinit() noexcept
    {
        log::debug(COMPONENT, "Destroying BLE context.");

        nimble_port_freertos_deinit();
        nimble_port_stop();
        nimble_port_deinit();

        if (esp_err_t result = esp_nimble_hci_and_controller_deinit();
            result != ESP_OK)
        {
            log::warning(COMPONENT,
                "HCI and controller deinitialization failed. Error code: {}.",
                result);
            return tl::make_unexpected(result);
        }

        return {};
    }
} // namespace b2h::ble
