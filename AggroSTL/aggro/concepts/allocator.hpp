#ifndef ALLOCCONCEPTS_HPP
#define ALLOCCONCEPTS_HPP

#include <concepts>
#include "objects.hpp"

namespace aggro
{
    template<typename T>
    concept pointer_to_buffer = pointer<T> && std::is_array_v<T>;

    template<typename T>
    concept standard_allocator = requires (T type, size_t size)
    {
        pointer_to_buffer<decltype(type.m_buffer)>;
        { type.allocate(size) };
        { type.deallocate(size) };
        { type.deallocate(type.m_buffer, size) };
    };

} // namespace aggro

#endif // ALLOCCONCEPTS_HPP