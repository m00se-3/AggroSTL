#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/allocator.hpp"

namespace aggro
{
    template<typename T>
    struct std_allocator
    {
        using size_type = std::size_t;
        T* m_buffer = nullptr;

        void allocate(size_type amount)
        {
            m_buffer = static_cast<T*>(::operator new(amount * sizeof(T)));
        }

        void deallocate(size_type amount)
        {
            ::operator delete(m_buffer, amount * sizeof(T));
        }

        void deallocate(T* start, size_type amount)
        {
            ::operator delete(start, amount * sizeof(T));
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP