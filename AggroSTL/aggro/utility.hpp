#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "concepts/objects.hpp"

namespace aggro
{
    template<typename T> requires (pointer<T> == false)
    inline constexpr T&& move(const T& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    /*
        A very generic structure holding a pair of values.
    */
    template<default_constructible T, default_constructible U>
    struct pair
    {
        T first;
        T second;

        constexpr pair() = default;
        constexpr ~pair() = default;

        constexpr pair(const T& t, const U& u) : first(t), second(u) {}
        constexpr pair(T&& t, U&& u) : first(move(t)), second(move(u)) {}

        constexpr pair(const pair& other) : first(other.first), second(other.second) {}
        constexpr pair(pair&& other) : first(move(other.first)), second(move(other.second)) {}

    };

    template<default_constructible T, default_constructible U>
    inline constexpr pair<T,U> make_pair(T&& t, U&& u)
    {
        return pair(move(t), (move(u));
    }

} // namespace aggro


#endif // UTILITY_HPP