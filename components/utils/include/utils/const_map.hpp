#ifndef B2H_UTILS_CONST_MAP_HPP
#define B2H_UTILS_CONST_MAP_HPP

#include <algorithm>
#include <cstdint>
#include <iterator>

#ifndef NO_EXCEPTIONS
#include <stdexcept>
#endif

#ifndef B2H_UTILS_CONST_MAP_DISABLE_REVERSE_ITERATION
#define B2H_UTILS_CONST_MAP_DISABLE_REVERSE_ITERATION
#endif

namespace b2h::utils
{
    template<typename T>
    class _const_map_iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        constexpr _const_map_iterator() noexcept : ptr{ nullptr } { }

        constexpr explicit _const_map_iterator(pointer ptr) noexcept :
            ptr{ ptr }
        {
        }

        constexpr _const_map_iterator(const _const_map_iterator& other) noexcept
            :
            ptr{ other.ptr }
        {
        }

        ~_const_map_iterator() = default;

        constexpr _const_map_iterator& operator=(
            const _const_map_iterator& other) noexcept
        {
            ptr = other.ptr;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(
            const _const_map_iterator& other) const noexcept
        {
            return (ptr == other.ptr);
        }

        [[nodiscard]] constexpr bool operator!=(
            const _const_map_iterator& other) const noexcept
        {
            return (ptr != other.ptr);
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return ptr;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return *(operator->());
        }

        constexpr _const_map_iterator& operator++() noexcept
        {
            ++ptr;
            return *this;
        }

        constexpr _const_map_iterator operator++(int) noexcept
        {
            _const_map_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr _const_map_iterator& operator--() noexcept
        {
            --ptr;
            return *this;
        }

        constexpr _const_map_iterator operator--(int) noexcept
        {
            _const_map_iterator tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        pointer ptr;
    };

    template<typename Key, typename T, std::size_t Size,
        typename Compare = std::less<Key>>
    struct const_map {
        using key_type               = const Key;
        using mapped_type            = const T;
        using key_compare            = Compare;
        using value_type             = const std::pair<key_type, mapped_type>;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using pointer                = value_type*;
        using const_pointer          = pointer;
        using reference              = value_type&;
        using const_reference        = reference;
        using iterator               = _const_map_iterator<value_type>;
        using const_iterator         = iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        value_type _data[Size];

        /**
         * @brief Returns an the number of elements.
         *
         * @return constexpr size_type
         */
        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return Size;
        }

        /**
         * @brief Returns an pointer to the first element.
         *
         * @return constexpr const_pointer
         */
        [[nodiscard]] constexpr const_pointer data() const noexcept
        {
            return _data;
        }

        /**
         * @brief Returns an iterator to the beginning.
         *
         * @return constexpr const_iterator
         */
        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return const_iterator(_data);
        }

        /**
         * @brief Returns an iterator to the beginning.
         *
         * @return constexpr const_iterator
         */
        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return begin();
        }

#ifndef B2H_UTILS_CONST_MAP_DISABLE_REVERSE_ITERATION

        /**
         * @brief Returns a reverse iterator to the beginning.
         *
         * @return constexpr const_reverse_iterator
         */
        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

#endif

        /**
         * @brief Returns an iterator to the end.
         *
         * @return constexpr const_iterator
         */
        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return const_iterator(_data + Size);
        }

        /**
         * @brief Returns an iterator to the end.
         *
         * @return constexpr const_iterator
         */
        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return end();
        }

#ifndef B2H_UTILS_CONST_MAP_DISABLE_REVERSE_ITERATION

        /**
         * @brief Returns a reverse iterator to the end.
         *
         * @return constexpr const_reverse_iterator
         */
        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(cend());
        }

#endif

        /**
         * @brief Checks if a const_map object is sorted.
         *
         * @return true
         * @return false
         */
        [[nodiscard]] constexpr bool is_sorted() const noexcept
        {
            key_compare pred{};
            auto left{ cbegin() };
            auto right{ std::next(left) };

            while (right != cend())
            {
                if (!pred(left->first, right->first))
                {
                    return false;
                }

                left = right++;
            }

            return true;
        }

        /**
         * @brief Finds element by key. Returns iterator to the end if the key
         * is not found.
         *
         * @param key
         * @return constexpr const_iterator
         */
        [[nodiscard]] constexpr const_iterator find(
            const key_type& key) const noexcept
        {
            for (auto it = cbegin(); it != cend(); ++it)
            {
                if (it->first == key)
                {
                    return it;
                }
            }

            return cend();
        }

        /**
         * @brief Accesses element by key. Not range checked. Undefined
         * behaviour when key is not found.
         *
         * @param key
         * @return constexpr const mapped_type&
         */
        [[nodiscard]] constexpr const mapped_type& operator[](
            const key_type& key) const noexcept
        {
            return (find(key))->second;
        }

        /**
         * @brief Accesses element by key. Range checked.
         *
         * @param key
         * @return constexpr const mapped_type&
         * @throws std::out_of_range if the key is not found.
         */
        [[nodiscard]] constexpr const mapped_type& at(const key_type& key) const
        {
            auto iter = find(key);

            if (iter == cend())
            {
#ifdef NO_EXCEPTIONS
                std::abort();
#else
                throw std::out_of_range("Key not found.");
#endif
            }

            return iter->second;
        }

        /**
         * @brief Checks if an element is present under the given key.
         *
         * @param key
         * @return true
         * @return false
         */
        [[nodiscard]] constexpr bool contains(
            const key_type& key) const noexcept
        {
            return (find(key) != cend());
        }
    };

    /**
     * @brief Creates a const_map object with size deduced from the number of
     * arguments.
     *
     * @tparam KeyT
     * @tparam ValueT
     * @tparam ArgsT
     * @param args
     * @return constexpr const_map<KeyT, ValueT, sizeof...(ArgsT)>
     */
    template<typename KeyT, typename ValueT, typename... ArgsT>
    inline constexpr const_map<KeyT, ValueT, sizeof...(ArgsT)> make_const_map(
        ArgsT&&... args) noexcept
    {
        return { { std::forward<ArgsT>(args)... } };
    }
} // namespace b2h::utils

#endif
