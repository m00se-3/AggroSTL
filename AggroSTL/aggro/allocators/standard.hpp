#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/allocator.hpp"

namespace aggro
{
    template<typename T>
    struct std_allocator
    {
        using size_type = std::size_t;
        using memory_resource = T*;

        memory_resource m_buffer = nullptr;

        memory_resource resource() { return m_buffer; }

        const memory_resource resource() const { return m_buffer; }

        void set_res(memory_resource other) { m_buffer = other; }

        memory_resource allocate(size_type amount)
        {
            m_buffer = static_cast<memory_resource>(::operator new[](amount * sizeof(T)));

            return m_buffer;
        }

        void deallocate(memory_resource start, size_type size)
        {
            ::operator delete[](start, size * sizeof(T));
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP