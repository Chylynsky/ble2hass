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

#ifndef B2H_EVENT_TYPE_TRAITS_HPP
#define B2H_EVENT_TYPE_TRAITS_HPP

#include <cstdint>
#include <type_traits>
#include <variant>

namespace b2h::event
{
    template<typename T, typename... ArgsT>
    inline constexpr bool is_any_of_v =
        std::disjunction_v<std::is_same<T, ArgsT>...>;

    template<typename, typename = void>
    struct has_argument_type : std::false_type {
    };

    template<typename T>
    struct has_argument_type<T, std::void_t<typename T::argument_type>> :
        std::true_type {
    };

    template<typename T>
    inline constexpr bool has_argument_type_v = has_argument_type<T>::value;

    template<typename, typename = void>
    struct has_error_type : std::false_type {
    };

    template<typename T>
    struct has_error_type<T, std::void_t<typename T::error_type>> :
        std::true_type {
    };

    template<typename T>
    inline constexpr bool has_error_type_v = has_error_type<T>::value;

    // clang-format off
template<typename T>
inline constexpr bool is_event_type_v = 
    has_argument_type_v<T>                                   && 
    has_error_type_v<T>                                      && 
    (std::is_void_v<typename T::argument_type>               || 
    std::is_move_constructible_v<typename T::argument_type>) &&
    std::is_move_constructible_v<typename T::error_type>     &&
    !std::is_reference_v<typename T::argument_type>          &&
    !std::is_reference_v<typename T::error_type>;
    // clang-format on

    template<typename T>
    inline constexpr bool is_valid_handler = std::is_move_constructible_v<T>;

    template<typename T, typename... ArgsT>
    struct index_of;

    template<typename T, typename... ArgsT>
    struct index_of<T, T, ArgsT...> : std::integral_constant<std::size_t, 0> {
    };

    template<typename T, typename V, typename... ArgsT>
    struct index_of<T, V, ArgsT...> :
        std::integral_constant<std::size_t, 1 + index_of<T, ArgsT...>::value> {
    };

    template<typename T, typename... ArgsT>
    inline constexpr std::size_t index_of_v = index_of<T, ArgsT...>::value;
} // namespace b2h::event

#endif
