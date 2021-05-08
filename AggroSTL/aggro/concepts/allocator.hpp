#ifndef ALLOCCONCEPTS_HPP
#define ALLOCCONCEPTS_HPP

#include <concepts>
#include "objects.hpp"

namespace aggro
{
    template<typename T>
    concept pointer_to_buffer = pointer<T> && std::is_array_v<T>;

    template<typename T>
    concept allocator = requires (T t)
    {
        pointer_to_buffer<decltype(t.m_buffer)>;
    };
    
    template<typename T>
    concept standard_allocator = allocator<T> && requires (T type, size_t size)
    {
        { type.allocate(size) };
        { type.deallocate(type.m_buffer, size) };
    };

} // namespace aggro

#endif // ALLOCCONCEPTS_HPP