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

    private:
        memory_resource m_buffer = nullptr; //Pointer to the beginning of the memory buffer.

    public:
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
    
    private:
        memory_resource m_head_node = nullptr; //Pointer to the head node.
        memory_resource m_tail_node = nullptr; //Pointer to the tail node;

    public:
        //Returns a pointer to the head node.
        constexpr memory_resource resource() { return m_head_node; }

        //Returns a pointer to the head node.
        constexpr const memory_resource resource() const { return m_head_node; }

        //Returns a pointer to the tail node.
        constexpr memory_resource resource_rev() { return m_tail_node; }

        //Returns a pointer to the tail node.
        constexpr const memory_resource resource_rev() const { return m_tail_node; }

        //Changes the head and/ or tail nodes provided the respective arguments are not nullptr.
        constexpr void set_res(memory_resource first, memory_resource last = nullptr)
        {
            if(first) m_head_node = first;
            if(last) m_tail_node = last;
        }

        //Remove all pointers from this allocator.
        constexpr void unlink()
        {
            m_head_node = nullptr;
            m_tail_node = nullptr;
        }

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

        //Constructs an object into the specified location using placement new.
        template<typename... Args>
        constexpr void construct(value_type* spot, Args&&... args)
        {
            new(spot) value_type(forward<Args>(args)...);
        }
    };

} // namespace aggro

#endif // STDALLOCATOR_HPP
