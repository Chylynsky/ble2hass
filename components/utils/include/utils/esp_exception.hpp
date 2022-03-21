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

#ifndef B2H_ESP_EXCEPTION_HPP
#define B2H_ESP_EXCEPTION_HPP

#include <stdexcept>
#include <string_view>

#include "esp_err.h"

/**
 * @brief Log an error and throw expection.
 *
 */
#define LOG_AND_THROW(tag, except)            \
    do                                        \
    {                                         \
        auto __except = except;               \
        ESP_LOGE(tag, "%s", __except.what()); \
        throw std::move(__except);            \
    } while (false);

namespace b2h::utils
{
    /**
     * @brief Exception holding error code of type esp_err_t besides an error
     * message.
     *
     */
    struct esp_exception : public std::runtime_error {
        explicit esp_exception(
            std::string_view message, esp_err_t errc = ESP_FAIL);

        /**
         * @brief Returns an error code.
         *
         * @return esp_err_t
         */
        esp_err_t errc() const noexcept;

        esp_err_t _errc;
    };
} // namespace b2h::utils

#endif
