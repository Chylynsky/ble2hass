#ifndef B2H_UTILS_FSM_HPP
#define B2H_UTILS_FSM_HPP

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <variant>

#ifndef B2H_FSM_NOEXCEPT
#ifndef NO_EXCEPTIONS
#define B2H_FSM_NOEXCEPT
#else
#define B2H_FSM_NOEXCEPT noexcept
#endif
#endif

namespace b2h::utils
{
    /**
     * @brief Tag for state check in fsm::get_state function.
     *
     */
    struct state_check {
    };

    /**
     * @brief Tag for omitting the state check in fsm::get_state function.
     *
     */
    struct no_state_check {
    };

    /**
     * @brief Checks if the given type is either state_check or no_state_check
     * type.
     *
     * @tparam StateCheckT
     */
    template<typename StateCheckT>
    inline constexpr bool is_state_check_type_v =
        std::is_same_v<StateCheckT, state_check> ||
        std::is_same_v<StateCheckT, no_state_check>;

    /**
     * @brief Finite state machine implementation based on std::variant.
     *
     */
    template<typename... StatesT>
    class fsm
    {
    public:
        static_assert(
            std::conjunction_v<std::is_move_constructible<StatesT>...>,
            "States are not move constructible.");
        static_assert(std::conjunction_v<std::is_move_assignable<StatesT>...>,
            "States are not move assignable.");

        /**
         * @brief Variant of states.
         *
         */
        using state_t = std::variant<StatesT...>;

        /**
         * @brief Checks if a given type is a state type.
         *
         * @tparam T
         */
        template<typename T>
        static inline constexpr bool is_state_type_v =
            std::disjunction_v<std::is_same<T, StatesT>...>;

        /**
         * @brief Construct a new fsm object.
         *
         * @tparam StateT
         * @param init_state
         */
        template<typename StateT>
        fsm(StateT&& init_state) : m_state{ std::move(init_state) }
        {
        }

        fsm() = default;

        fsm(const fsm&) = default;

        fsm(fsm&&) = default;

        fsm& operator=(const fsm&) = default;

        fsm& operator=(fsm&&) = default;

        ~fsm() = default;

        /**
         * @brief Checks if fsm object is currently in StateT state.
         *
         * @tparam StateT
         * @return true
         * @return false
         */
        template<typename StateT>
        bool is_state() const noexcept
        {
            static_assert(is_state_type_v<StateT>, "Not a valid state type.");
            return std::holds_alternative<StateT>(m_state);
        }

        /**
         * @brief Set the state. State object is constructed in place.
         *
         * @tparam StateT
         * @tparam ArgsT
         * @param args
         * @return StateT&
         */
        template<typename StateT, typename... ArgsT>
        StateT& set_state(ArgsT&&... args) noexcept
        {
            static_assert(is_state_type_v<StateT>, "Not a valid state type.");
            static_assert(std::is_constructible_v<StateT, ArgsT...>,
                "State is not constructible with supplied arguments.");
            m_state = StateT(std::forward<ArgsT>(args)...);
            return std::get<StateT>(m_state);
        }

        /**
         * @brief Get the current state.
         *
         * NOTE: Expected state must be provided by the template parameter.
         *
         * @tparam StateT
         * @tparam StateCheckT
         * @return StateT&
         */
        template<typename StateT, typename StateCheckT = state_check>
        StateT& get_state() B2H_FSM_NOEXCEPT
        {
            static_assert(is_state_type_v<StateT>, "Not a valid state type.");
            static_assert(is_state_check_type_v<StateCheckT>,
                "Available options for state check are: state_check and no_state_check.");

            if constexpr (std::is_same_v<StateCheckT, state_check>)
            {
                throw_on_invalid_state<StateT>();
            }

            return std::get<StateT>(m_state);
        }

    private:
        state_t m_state;

        template<typename StateT>
        void throw_on_invalid_state() B2H_FSM_NOEXCEPT
        {
            static_assert(is_state_type_v<StateT>, "Not a valid state type.");
            if (!is_state<StateT>())
            {
#ifndef NO_EXCEPTIONS
                throw std::logic_error("Invalid state.");
#else
                abort();
#endif
            }
        }
    };
} // namespace b2h::utils

#endif