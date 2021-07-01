#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include "concepts/objects.hpp"

namespace aggro
{
    // Used as a helper struct when constructing null optionals.
    // (Not required)
    struct nullopt_t
    {
        constexpr nullopt_t() {}
    };

    inline constexpr nullopt_t nullopt{};

    /*
        A class that represents a value that may not exist.
        Used for returning from operations that may fail.
    */
    template<destructible T>
    class optional
    {
    public:
        using value_type = T;
    
    private:
        value_type val;
        bool value_set = false;

    public:
        constexpr optional() = default;
        constexpr ~optional() = default;

        constexpr optional(const T& value)
        : val(value), value_set(true)
        {}

        constexpr optional(T&& value)
        : val(move(value)), value_set(true)
        {}

        constexpr optional(const nullopt_t&) {}

        template<destructible U>
        constexpr explicit optional(const optional<U>& other) 
        {
            if(other)
            {
                emplace(other.value());
                value_set = true;
            }
        }

        template<destructible U>
        constexpr explicit optional(optional<U>&& other) noexcept
        {
            if(other)
            {
                emplace(other.value());
                value_set = true;
            }
        }

        template<destructible U = value_type>
        constexpr optional(U&& v) : val(v), value_set(true) {}

        constexpr optional& operator=(const optional& other)
        {
            if(other)
            {
                emplace(other.value());
            }

            return *this;
        }

        template<destructible U = value_type>
        constexpr optional& operator=(const U& v)
        {
            reset();
            val = v;
            value_set = true;

            return *this;
        }

        constexpr T* operator->() { return &val; }

        constexpr const T* operator->() const { return &val; }

        constexpr T& operator*() { return val; }

        constexpr const T& operator*() const { return val; }

        constexpr explicit operator bool() const { return value_set; }

        constexpr T& value() { return val; }

        constexpr const T& value() const { return val; }

        constexpr T& value_or(T&& v) { return (value_set) ? val : v; }

        constexpr const T& value_or(T&& v) const { return (value_set) ? val : v; }

        constexpr bool has_value() const { return value_set; }

        constexpr void reset()
        {
            if(value_set)
            {
                value_set = false;
                val.~T();
            }
        }

        template<typename... Args>
        constexpr T& emplace(Args&&... args) 
        {
            reset();

            new(&val) T(std::forward<Args&&>(args)...);

            return val;
        }
    };

    template<destructible T, typename... Args>
    inline constexpr optional<T> make_optional(Args&&... args)
    {
        return optional<T>(T(forward<Args>(args)...));
    }

    template<destructible T>
    inline constexpr optional<T> make_optional(T&& val)
    {
        return optional<T>(move(val));
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator==(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() == rhs.value());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() != rhs.value());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() >= rhs.value());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() <= rhs.value());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator>(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() > rhs.value());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator<(const optional<T>& lhs, const optional<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.value() < rhs.value());
        }

        return false;
    }

} // namespace aggro


#endif // OPTIONAL_HPP