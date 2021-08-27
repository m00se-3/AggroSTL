#ifndef OBJECTLIFE_HPP
#define OBJECTLIFE_HPP

#include <type_traits>

namespace aggro
{
    template<typename T, typename U>
    concept same = std::is_same_v<T, U>;

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
    
    template<typename T>
    concept pointer = std::is_pointer_v<T>;

    template<typename T>
    concept data_iterator = fully_comparable<T,T> && (pointer<T> || requires (T it, size_t i) {
        { ++it } -> data_iterator;
        { it++ } -> data_iterator;
        { --it } -> data_iterator;
        { it-- } -> data_iterator;
        { it + i } -> data_iterator;
        { it - i } -> data_iterator;
        { it += i } -> data_iterator;
        { it -= i } -> data_iterator;
    });
    
    template<typename T>
    concept destructible = std::is_destructible_v<T>;

    template<typename T>
    concept default_constructible = std::is_default_constructible_v<T>;

    template<typename T>
    concept iterator_enabled = requires (T type){
        typename T::iterator;
        typename T::const_iterator;
        { type.begin() } -> data_iterator;
        { type.end() } -> data_iterator;
    };

    template<typename T>
    concept reverse_iterator_enabled = iterator_enabled<T> &&
        requires (T type){
        typename T::reverse_iterator;
        typename T::const_reverse_iterator;
        { type.rbegin() } -> data_iterator;
        { type.rend() } -> data_iterator;
    };

    template<typename T> 
    concept iterator_disabled = !iterator_enabled<T>;

} // namespace aggro


#endif // OBJECTLIFE_HPP