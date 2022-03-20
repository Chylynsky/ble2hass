#ifndef B2H_DEVICE_FACTORY_HPP
#define B2H_DEVICE_FACTORY_HPP

#include <map>
#include <memory>
#include <string_view>

#include "ble/gatt/client.hpp"
#include "device/base.hpp"
#include "mqtt/client.hpp"

namespace b2h::device
{
    using factory_function_t = std::function<std::shared_ptr<interface>(
        std::unique_ptr<mqtt::client>&&, std::unique_ptr<ble::gatt::client>&&)>;

    extern const std::map<std::string_view, factory_function_t> g_factory_map;
} // namespace b2h::device

#endif
