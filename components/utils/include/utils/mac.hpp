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

#ifndef B2H_UTILS_MAC_HPP
#define B2H_UTILS_MAC_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "fmt/format.h"
#include "tcb/span.hpp"

namespace impl
{
    template<typename Iter>
    inline constexpr bool is_char_iter_v =
        std::is_same_v<typename std::iterator_traits<Iter>::value_type, char>;

    template<typename Iter>
    inline constexpr bool is_byte_iter_v =
        std::is_same_v<typename std::iterator_traits<Iter>::value_type,
            std::uint8_t>;

    template<typename T>
    inline constexpr bool always_false_v = false;
} // namespace impl

namespace b2h::utils
{
    class mac
    {
    public:
        static constexpr std::size_t MAC_SIZE{ 6 };
        static constexpr std::size_t MAC_STR_SIZE{ 17 }; // MAC string size

        using buffer_t = std::array<std::uint8_t, MAC_SIZE>;

        mac()           = default;
        mac(const mac&) = default;
        mac(mac&&)      = default;

        mac& operator=(const mac&) = default;
        mac& operator=(mac&&) = default;

        template<typename OutIt>
        std::optional<std::string_view> to_charbuf(
            OutIt first, OutIt last) const noexcept
        {
            static_assert(impl::is_char_iter_v<OutIt>,
                "Iterator value type must be char.");

            const auto [out, size] = fmt::format_to_n(first,
                static_cast<std::size_t>(std::distance(first, last)),
                "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                m_address[5],
                m_address[4],
                m_address[3],
                m_address[2],
                m_address[1],
                m_address[0]);

            if (size == 0)
            {
                return std::nullopt;
            }

            return std::string_view{ &(*first), size };
        }

        std::string to_string() const;

        tcb::span<const std::uint8_t> as_bytes() const noexcept
        {
            return tcb::make_span(m_address);
        }

        friend std::optional<mac> make_mac(const std::string_view str) noexcept;

        template<typename InIt>
        friend std::optional<mac> make_mac(InIt first, InIt last) noexcept;

    private:
        buffer_t m_address;
    };

    std::optional<mac> make_mac(const std::string_view str) noexcept;

    template<typename InIt>
    std::optional<mac> make_mac(InIt first, InIt last) noexcept
    {
        static_assert(impl::is_byte_iter_v<InIt>,
            "Required iterator with value_type std::uint8_t.");
        mac result;

        if (std::distance(first, last) != mac::MAC_SIZE)
        {
            return std::nullopt;
        }

        std::copy(first, last, result.m_address.begin());
        return result;
    }
} // namespace b2h::utils

#endif
