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
