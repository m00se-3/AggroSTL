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

    //Used as a helper struct when constructing optional_refs that are not referencing anything.
    //(Not required)
    template<typename T>
    struct nullopt_ref_t
    {
        T dummy_value {};
        constexpr nullopt_ref_t() {}
    };

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

        constexpr optional(const nullopt_t&) : val(value_type{}) {}

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
        constexpr optional(U&& v) : val(move(v)), value_set(true) {}

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



    /*
        A class that represents a reference to something that may not exist.
        This class is intended to create editable references to existing variables and
        container objects, such as an object in a darray.

        Warning!! You must ensure that the object being referenced out-lives the optional_ref!
        Attempting to dereference an optional_ref can be dangerous if the object it references no
        longer exists!
    */
    template<destructible T>
    class optional_ref
    {
    public:
        using value_type = T;
        using reference = T&;
    
    private:
        reference val;
        bool ref_set = false;

    public:
        constexpr optional_ref() = default;
        constexpr ~optional_ref() = default;

       constexpr optional_ref(T& value)
        : val(value), ref_set(true)
        {}

        constexpr optional_ref(nullopt_ref_t<T>&& nu) : val(nu.dummy_value) {}

        template<destructible U>
        constexpr explicit optional_ref(const optional_ref& other) = delete;

        template<destructible U>
        constexpr explicit optional_ref(optional_ref&& other) noexcept = delete;

        constexpr optional_ref& operator=(const optional_ref& other)
        {
            if(other)
            {
                val = other.ref();
            }

            return *this;
        }

        template<destructible U = value_type>
        constexpr optional_ref& operator=(U& v)
        {
            reset();
            val = v;
            ref_set = true;

            return *this;
        }

        constexpr T& operator*() { return val; }

        constexpr const T& operator*() const { return val; }

        constexpr explicit operator bool() const { return ref_set; }

        constexpr T& ref() { return val; }

        constexpr const T& ref() const { return val; }

        constexpr bool has_ref() const { return ref_set; }

        constexpr void reset()
        {
            ref_set = false;
        }
    };

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator==(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() == rhs.ref());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator!=(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() != rhs.ref());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator>=(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() >= rhs.ref());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator<=(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() <= rhs.ref());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator>(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() > rhs.ref());
        }

        return false;
    }

    template<typename T, typename U> requires fully_comparable<T, U>
    inline bool operator<(const optional_ref<T>& lhs, const optional_ref<U>& rhs)
    {
        if(lhs && rhs)
        {
            return (lhs.ref() < rhs.ref());
        }

        return false;
    }

    

} // namespace aggro


#endif // OPTIONAL_HPP