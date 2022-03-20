#include "utils/mac.hpp"

#include <charconv>

namespace b2h::utils
{
    std::string mac::to_string() const
    {
        return fmt::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
            m_address[5],
            m_address[4],
            m_address[3],
            m_address[2],
            m_address[1],
            m_address[0]);
    }

    std::optional<mac> make_mac(const std::string_view str) noexcept
    {
        mac result;

        std::uint16_t buf;
        std::from_chars_result res;

        const char* first = str.data();

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[5] = buf;

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[4] = buf;

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[3] = buf;

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[2] = buf;

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[1] = buf;

        if (res = std::from_chars(first, first + 2, buf, 16);
            res.ec != std::errc{})
        {
            return std::nullopt;
        }

        first               = res.ptr + 1;
        result.m_address[0] = buf;

        return result;
    }

} // namespace b2h::utils
