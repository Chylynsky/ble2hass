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

        mac()           = default;
        mac(const mac&) = default;
        mac(mac&&)      = default;

        mac& operator=(const mac&) = default;
        mac& operator=(mac&&) = default;

        template<typename OutIt>
        void to_charbuf(OutIt first, OutIt last) const noexcept
        {
            static_assert(impl::is_char_iter_v<OutIt>,
                "Iterator value type must be char.");
            assert(std::distance(first, last) == MAC_STR_SIZE);

            fmt::format_to(first,
                "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                m_address[5],
                m_address[4],
                m_address[3],
                m_address[2],
                m_address[1],
                m_address[0]);
        }

        std::string to_string() const;

        const std::uint8_t* as_bytes() const noexcept
        {
            return m_address.data();
        }

        friend std::optional<mac> make_mac(const std::string_view str) noexcept;

        template<typename InIt>
        friend std::optional<mac> make_mac(InIt first, InIt last) noexcept;

    private:
        std::array<std::uint8_t, MAC_SIZE> m_address;
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
