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

#ifndef B2H_EVENT_BASIC_EVENT_HPP
#define B2H_EVENT_BASIC_EVENT_HPP

#include <type_traits>

#include "tl/expected.hpp"

namespace b2h::event
{
    /**
     * @brief Base template for all event types. Using this template ensures
     * that the template instantiation satisfies the Event concept.
     *
     * Event concept is satisfied when all of the below conditions are valid:
     * - argument type is not a reference but can be void
     * - error_type is move constructible
     * - neither argument_type or error_type is a reference
     *
     * @tparam ArgsT Type of the argument
     * @tparam ErrorT Type of the error
     */
    template<typename ArgsT, typename ErrorT>
    struct basic_event {
        using argument_type = ArgsT;
        using error_type    = ErrorT;

        static_assert(std::is_void_v<argument_type> ||
                          std::is_move_constructible_v<argument_type>,
            "argument_type must be move constructible or void.");
        static_assert(!std::is_reference_v<argument_type>,
            "argument_type must not be a reference.");
        static_assert(std::is_move_constructible_v<error_type>,
            "error_type must be move constructible.");
        static_assert(!std::is_reference_v<error_type>,
            "error_type must not be a reference.");

        using expected_type = tl::expected<argument_type, error_type>;
    };
} // namespace b2h::event

#endif
