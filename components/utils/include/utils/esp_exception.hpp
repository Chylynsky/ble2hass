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
        explicit esp_exception(std::string_view message,
            esp_err_t errc = ESP_FAIL);

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
