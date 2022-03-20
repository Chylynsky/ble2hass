#ifndef B2H_BLE_BLE_HPP
#define B2H_BLE_BLE_HPP

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"

#include "services/gap/ble_svc_gap.h"

#include "esp_err.h"
#include "esp_nimble_hci.h"
#undef min
#undef max

#include <string_view>

#include "tl/expected.hpp"

#include "event/event.hpp"
#include "utils/logger.hpp"

namespace b2h
{
    namespace events::ble
    {
        struct init_args {
        };

        using init = event::basic_event<init_args, esp_err_t>;

    } // namespace events::ble

    namespace ble
    {
        namespace impl
        {
            extern std::function<void(void)> g_on_sync;

            void on_sync() noexcept;
        } // namespace impl

        class context
        {
        public:
            using dispatcher_type = event::dispatcher<events::ble::init>;
            using receiver_type   = dispatcher_type::receiver_type;

            explicit context(event::context& ctx) noexcept;

            context(const context&) = delete;

            context(context&& other) noexcept;

            context& operator=(const context&) = delete;

            context& operator=(context&& other) noexcept;

            context() = delete;

            ~context() = default;

            template<typename HandlerT>
            void async_init(HandlerT handler) noexcept
            {
                log::debug(COMPONENT, "Initializing BLE context.");

                m_receiver.template async_receive<events::ble::init>(
                    std::move(handler));

                if (esp_err_t result = esp_nimble_hci_and_controller_init();
                    result != ESP_OK)
                {
                    log::error(COMPONENT,
                        "HCI and controller initialization failed. Error code: {}.",
                        result);
                    m_dispatcher.template async_dispatch<events::ble::init>(
                        tl::make_unexpected(result));
                }

                log::info(COMPONENT,
                    "HCI and controller initialized successfully.");

                nimble_port_init();

                impl::g_on_sync = [this]() {
                    m_dispatcher.template async_dispatch<events::ble::init>({});
                };

                ble_hs_cfg.sync_cb = &impl::on_sync;

                nimble_port_freertos_init([](void*) { nimble_port_run(); });
            }

            tl::expected<void, esp_err_t> deinit() noexcept;

        private:
            friend void impl::on_sync() noexcept;

            static constexpr std::string_view COMPONENT{ "ble::context" };

            dispatcher_type m_dispatcher;
            receiver_type m_receiver;
        };

    } // namespace ble
} // namespace b2h

#endif
