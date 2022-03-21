#ifndef B2H_UTILS_LOGGER_HPP
#define B2H_UTILS_LOGGER_HPP

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

#include "fmt/color.h"
#include "fmt/format.h"

#ifndef B2H_LOG_LEVEL
#define B2H_LOG_LEVEL log_level::verbose
#endif

namespace b2h::log
{
    enum class log_level : std::uint8_t
    {
        none,
        critical,
        error,
        warning,
        info,
        debug,
        verbose,
    };

    inline constexpr auto CRITICAL_COLOR = fmt::color::dark_red;
    inline constexpr auto ERROR_COLOR    = fmt::color::red;
    inline constexpr auto WARNING_COLOR  = fmt::color::yellow;
    inline constexpr auto INFO_COLOR     = fmt::color::green;
    inline constexpr auto VERBOSE_COLOR  = fmt::color::light_gray;

    inline constexpr auto GLOBAL_LOG_LEVEL = B2H_LOG_LEVEL;

    template<typename... ArgsT>
    inline void verbose([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::verbose))
        {
            return;
        }

        fmt::print(fmt::fg(VERBOSE_COLOR),
            "VERBOSE  | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }

    template<typename... ArgsT>
    inline void debug([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::debug))
        {
            return;
        }

        fmt::print("DEBUG    | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }

    template<typename... ArgsT>
    inline void info([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::info))
        {
            return;
        }

        fmt::print(fmt::fg(INFO_COLOR),
            "INFO     | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }

    template<typename... ArgsT>
    inline void warning([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::warning))
        {
            return;
        }

        fmt::print(fmt::fg(WARNING_COLOR),
            "WARNING  | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }

    template<typename... ArgsT>
    inline void error([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::error))
        {
            return;
        }

        fmt::print(fmt::fg(ERROR_COLOR),
            "ERROR    | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }

    template<typename... ArgsT>
    inline void critical([[maybe_unused]] std::string_view component,
        [[maybe_unused]] std::string_view format,
        [[maybe_unused]] ArgsT... args) noexcept
    {
        if constexpr (static_cast<uint8_t>(GLOBAL_LOG_LEVEL) <
                      static_cast<uint8_t>(log_level::critical))
        {
            return;
        }

        fmt::print(fmt::fg(CRITICAL_COLOR),
            "CRITICAL | {:<17} | {}\n",
            component,
            fmt::format(format, std::forward<ArgsT>(args)...));
    }
} // namespace b2h::log

#endif
