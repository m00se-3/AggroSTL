#ifndef STDALLOCATOR_HPP
#define STDALLOCATOR_HPP

#include "../concepts/allocator.hpp"
#include "../utility.hpp"

namespace aggro
{
    template<typename T>
    struct std_contiguous_allocator
    {
        using size_type = std::size_t;
        using memory_resource = T*;
        using value_type = T;

        memory_resource m_buffer = nullptr; //Pointer to the beginning of the memory buffer.

        //Returns a pointer to the memory resource.
        constexpr memory_resource resource() { return m_buffer; }

        //Returns a pointer to the memory resource.
        constexpr const memory_resource resource() const { return m_buffer; }

        //Sets the underlying pointer to a new memory buffer.
        constexpr void set_res(memory_resource other) { m_buffer = other; }

        //Allocate a new memory buffer.
        [[nodiscard]] constexpr memory_resource allocate(size_type amount)
        {
            return static_cast<memory_resource>(::operator new[](amount * sizeof(T)));
        }

        //Deallocate a memory resource, specifying the size of the buffer.
        constexpr void deallocate(memory_resource start, size_type size)
        {
            ::operator delete[](start, size * sizeof(T));
        }
        
        //Construct an object at the specified location using placement new.
        template<typename... Args>
        constexpr void construct(memory_resource spot, Args&&... args)
        {
            new(spot) value_type(forward<Args>(args)...);
        }
    };

    template<typename T>
    struct std_node_allocator
    {
        using size_type = std::size_t;
        using memory_resource = T*;
        using value_type = typename T::value_type;

        memory_resource m_head_node = nullptr; //Pointer to the head node.

        //Returns a pointer to the head node.
        constexpr memory_resource resource() { return m_head_node; }

        //Returns a pointer to the head node.
        constexpr const memory_resource resource() const { return m_head_node; }

        //Changes the node that is pointed to as the head.
        constexpr void set_res(memory_resource other) { m_head_node = other; }

        //Allocates a new node and returns a pointer to it.
        [[nodiscard]] constexpr memory_resource allocate(size_type amount)
        {
            return static_cast<memory_resource>(::operator new[](amount * sizeof(T)));
        }

        //Deallocates a node.
        constexpr void deallocate(memory_resource start, size_type size)
        {
            ::operator delete[](start, size * sizeof(T));
        }
        
        //Constructs an object into the specified node using placement new.
        template<typename... Args>
        constexpr void construct(memory_resource spot, Args&&... args)
        {
            new(&(spot->value)) value_type(forward<Args>(args)...);
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP
