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

#ifndef B2H_DEVICE_FACTORY_HPP
#define B2H_DEVICE_FACTORY_HPP

#include <map>
#include <memory>
#include <string_view>
#include <variant>

#include "ble/gatt/client.hpp"
#include "device/base.hpp"
#include "mqtt/client.hpp"

// Include device implementation headers

#include "xiaomi/lywsd03mmc.hpp"
#include "xiaomi/mikettle.hpp"

namespace b2h::device
{
    using device_variant_t = std::variant<xiaomi::mikettle, xiaomi::lywsd03mmc>;

    using factory_function_t =
        std::function<device_variant_t(mqtt::client, ble::gatt::client)>;

    extern const std::map<std::string_view, factory_function_t> g_factory_map;
} // namespace b2h::device

#endif
