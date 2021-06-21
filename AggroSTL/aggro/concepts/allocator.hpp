#ifndef ALLOCCONCEPTS_HPP
#define ALLOCCONCEPTS_HPP

#include "objects.hpp"

namespace aggro
{    
    template<typename T>
    concept allocator = requires (T type, size_t size)
    {
        typename T::memory_resource;
        { type.allocate(size) } -> pointer;
        { type.deallocate(type.resource(), size) };
    };
    
    template<typename T>
    concept standard_allocator = allocator<T> && requires (T type) { { type.resource() } -> pointer; };

} // namespace aggro

#endif // ALLOCCONCEPTS_HPP