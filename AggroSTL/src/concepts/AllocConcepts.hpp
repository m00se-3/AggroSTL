#ifndef ALLOCCONCEPTS_HPP
#define ALLOCCONCEPTS_HPP

#include <concepts>
#include <type_traits>

namespace aggro
{
    template<typename T>
    concept Pointer = std::is_pointer_v<T>;

    template<typename T>
    concept PointerToBuffer = Pointer<T> && std::is_array_v<T>;

    template<typename T>
    concept StandardAllocator = requires (T type, size_t size)
    {
        PointerToBuffer<decltype(type.Buffer)>;
        { type.Allocate(size) };
        { type.Deallocate(size) };
        { type.Deallocate(type.Buffer, size) };
    };

} // namespace aggro

#endif // ALLOCCONCEPTS_HPP