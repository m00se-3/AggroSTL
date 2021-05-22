#ifndef OBJECTLIFE_HPP
#define OBJECTLIFE_HPP

#include <type_traits>

namespace aggro
{
    template<typename T, typename U>
    concept same = std::is_same_v<T, U>;
    
    template<typename T>
    concept pointer = std::is_pointer_v<T>;
    
    template<typename T>
    concept destructible = std::is_destructible_v<T>;

    template<typename T>
    concept default_constructible = std::is_default_constructible_v<T>;

    template<typename T, typename U>
    concept fully_comparable = requires(T t, U u)
    {
        { t == u } -> same<bool>;
        { t != u } -> same<bool>;
        { t < u } -> same<bool>;
        { t > u } -> same<bool>;
        { t <= u } -> same<bool>;
        { t >= u } -> same<bool>;
    };

} // namespace aggro


#endif // OBJECTLIFE_HPP