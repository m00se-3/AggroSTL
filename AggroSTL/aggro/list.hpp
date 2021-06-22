#ifndef LIST_HPP
#define LIST_HPP

#include <initializer_list>
#include "utility.hpp"
#include "concepts/stream.hpp"
#include "allocators/standard.hpp"

namespace aggro
{
    
    template<typename T>
    struct snode
    {
        using value_type = T;
        
        T value {};
        snode* next = nullptr;

        constexpr snode() = default;
        constexpr snode(const T& val) : value(val) {}
        constexpr snode(T&& val) : value(move(val)) {}
        constexpr snode(const T& val, snode* n) : value(val), next(n) {}
        constexpr snode(T&& val, snode* n) : value(move(val)), next(n) {}
        constexpr ~snode() = default;
    };
    
    /*
        A singley-linked list.
    */
    template<typename T, standard_allocator Alloc = std_node_allocator<snode<T>>>
    class slist
    {
        struct _iterator
        {  
            using value_type = T;
            
            snode* node;

            constexpr T& operator*()
            {
                return node->value;
            }

            constexpr const T& operator*() const
            {
                return node->value;
            }

            constexpr _iterator& operator+(size_type index)
            {
                while(this->node && index != 0)
                {
                    this->node = this->node->next;
                    --index;
                }

                return *this;
            }

            constexpr T& operator++() const //prefix
            {
                node = node->next;
                return node->value;
            }

            constexpr T operator++(int) const //postfix
            {
                T old = node->value;
                node = node->next;
                return old->value;
            };

            constexpr operator==(const _iterator& rhs)
            {
                if(this.node == rhs.node)
                {
                    return true;
                }
                return false;
            }
        };

    public:

        using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = _iterator;
        using const_iterator = const _iterator;
		using allocator_type = Alloc;

    private:

        snode* m_head = nullptr;
        size_type m_count = 0;
        allocator_type alloc;

        template<typename... Args>
        constexpr snode* _emplace(snode* spot, Args&&... args)
        {
            snode* new_node = alloc.allocate(1);
            new_node->next = spot;

            alloc.construct(&spot->value, forward<Args>(args)...);
            ++m_count;
            
            return new_node;
        }

    public:
        constexpr slist() = default;
        constexpr slist(const std::initializer_list<T>& init)
        {
            size_type init_size = init.size();
            snode* current = m_head;

            for(size_type i = 0; i < init_size; ++i)
            {
                if(current)
                {
                    current->next = _emplace(current->next, move(*(init.begin() + i)));
                }
                else
                {
                    current = _emplace(current, move(*(init.begin() + i)));
                }
                current = current->next;
            }
        }

        constexpr reference front() { return m_head->value; }

        constexpr const_reference front() const { return m_head->value; }

        constexpr void push_front(const T& value)
        {
            m_head = _emplace(m_head, value);
        }

        constexpr void push_front(T&& value)
        {
            m_head = _emplace(m_head, move(value));
        }

        template<typename... Args>
        constexpr void emplace_front(Args&&... args)
        {
            m_head = _emplace(m_head, forward<Args>(args)...);
        }

        constexpr void pop_front()
        {
            snode new_head = m_head->next;
            m_head->~snode();

            alloc.deallocate(m_head, 1);

            m_head = new_head;
        }

        constexpr ~slist() { clear(); }

        constexpr size_type size() const { return m_count; }

        constexpr void clear()
        {
            snode* node = m_head;

            while(node)
            {
                node* temp = snode;
                node = node->next;
                temp.~snode();
            }

            m_count = 0;
            m_head = nullptr;
        }

        constexpr iterator begin() { return _iterator{m_head}; }
        constexpr const_iterator begin() const { return _iterator{m_head}; }

        constexpr iterator end() { return iterator{nullptr}; }
        constexpr const_iterator end() const { return _iterator{nullptr}; }
        
        [[nodiscard("This function does not empty the list.")]] constexpr bool empty()
        {
            return (begin() == end());
        }

    };

} // namespace aggro


#endif // LIST_HPP