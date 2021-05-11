#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/allocator.hpp"

namespace aggro
{
    template<typename T>
    struct std_allocator
    {
        using size_type = std::size_t;
        using resource = T*;

        resource m_buffer = nullptr;

        resource res() { return m_buffer; }

        const resource res() const { return m_buffer; }

        void set_res(resource other) { m_buffer = other; }

        void allocate(size_type amount)
        {
            m_buffer = static_cast<T*>(::operator new(amount * sizeof(T)));
        }

        void deallocate(T* start, size_type amount)
        {
            ::operator delete(start, amount * sizeof(T));
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP