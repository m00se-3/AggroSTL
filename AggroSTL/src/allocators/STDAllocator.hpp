#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/AllocConcepts.hpp"

namespace aggro
{
    template<typename T>
    struct STDAllocator
    {
        T* Buffer = nullptr;

        void Allocate(size_t amount)
        {
            Buffer = static_cast<T*>(::operator new(amount * sizeof(T)));
        }

        void Deallocate(size_t amount)
        {
            ::operator delete(Buffer, amount * sizeof(T));
        }

        void Deallocate(T* start, size_t amount)
        {
            ::operator delete(start, amount * sizeof(T));
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP