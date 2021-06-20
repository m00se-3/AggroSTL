#ifndef ALLOCCONCEPTS_HPP
#define ALLOCCONCEPTS_HPP

#include "objects.hpp"

namespace aggro
{    
    template<typename T>
    concept allocator = requires (T type, size_t size)
    {
        typename T::resource;
        { type.allocate(size) };
        { type.deallocate(type.res()) };
    };
    
    template<typename T>
    concept standard_allocator = allocator<T> && requires (T type) { { type.res() } -> pointer; };

} // namespace aggro

#endif // ALLOCCONCEPTS_HPP