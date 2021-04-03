#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/AllocConcepts.hpp"

namespace aggro
{
    template<typename T>
    struct STDAllocator
    {
        using SizeType = std::size_t;
        T* Buffer = nullptr;

        void Allocate(SizeType amount)
        {
            Buffer = static_cast<T*>(::operator new(amount * sizeof(T)));
        }

        void Deallocate(SizeType amount)
        {
            ::operator delete(Buffer, amount * sizeof(T));
        }

        void Deallocate(T* start, SizeType amount)
        {
            ::operator delete(start, amount * sizeof(T));
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP