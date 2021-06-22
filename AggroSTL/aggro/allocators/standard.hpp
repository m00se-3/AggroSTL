#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/allocator.hpp"
#include "../utility.hpp"

namespace aggro
{
    template<typename T>
    struct std_allocator
    {
        using size_type = std::size_t;
        using memory_resource = T*;

        memory_resource m_buffer = nullptr;

        constexpr memory_resource resource() { return m_buffer; }

        constexpr const memory_resource resource() const { return m_buffer; }

        constexpr void set_res(memory_resource other) { m_buffer = other; }

        constexpr memory_resource allocate(size_type amount)
        {
            m_buffer = static_cast<memory_resource>(::operator new[](amount * sizeof(T)));

            return m_buffer;
        }

        constexpr void deallocate(memory_resource start, size_type size)
        {
            ::operator delete[](start, size * sizeof(T));
        }
        
        template<typename... Args>
        constexpr void construct(memory_resource spot, Args&&... args)
        {
            new(spot) T(forward<Args>(args)...);
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP