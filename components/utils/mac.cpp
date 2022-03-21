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
