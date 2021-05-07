#ifndef OBJECTLIFE_HPP
#define OBJECTLIFE_HPP

#include <concepts>

namespace aggro
{
    template<typename T>
    concept pointer = std::is_pointer_v<T>;
    
    template<typename T>
    concept destructible = std::destructible<T>;

    template<typename T, typename U>
    concept fully_comparable = requires(T t, U u)
    {
        { t == u } -> std::same_as<bool>;
        { t != u } -> std::same_as<bool>;
        { t < u } -> std::same_as<bool>;
        { t > u } -> std::same_as<bool>;
        { t <= u } -> std::same_as<bool>;
        { t >= u } -> std::same_as<bool>;
    };

} // namespace aggro


#endif // OBJECTLIFE_HPP