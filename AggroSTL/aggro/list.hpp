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

            constexpr snode* get() const { return node; }
            
            constexpr T& operator*()
            {
                return node->value;
            }

            constexpr const T& operator*() const
            {
                return node->value;
            }

            constexpr _iterator operator+(size_type index)
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

        allocator_type alloc;
        size_type m_count = 0;

        template<typename... Args>
        constexpr snode* _emplace(snode* spot, Args&&... args)
        {
            snode* new_node = alloc.allocate(1);
            new_node->next = spot;

            alloc.construct(spot, forward<Args>(args)...);
            ++m_count;
            
            return new_node;
        }

    public:
        constexpr slist() = default;
        constexpr slist(const std::initializer_list<T>& init)
        {
            size_type init_size = init.size();
            snode* current = alloc.resource();

            for(size_type i = 0; i < init_size; ++i)
            {
                if(alloc.resource() == nullptr)
                {
                    alloc.set_res(_emplace(nullptr, move(*(init.begin() + i))));
                    current = alloc.resource();
                }
                else
                {
                    current = _emplace(current, move(*(init.begin() + i)));
                }
                current = current->next;
            }
        }

        constexpr slist(const slist& other)
        {
            snode* current = alloc.resource();
            
            for(auto val : other)
            {
                if(alloc.resource() == nullptr)
                {
                    alloc.set_res(_emplace(nullptr, val));
                    current = alloc.resource();
                }
                else
                {
                    current = _emplace(current, val);
                }
                current = current->next;
            }
        }

        constexpr slist(slist&& other) noexcept
        : m_count(other.size())
        {
            allocator_type* o_all = other.get_allocator();
            alloc.set_res(o_all->resource());
            o_all->set_res(nullptr);
        }

        constexpr reference front() { return alloc.resource()->value; }

        constexpr const_reference front() const { return alloc.resource()->value; }

        constexpr void push_front(const T& value)
        {
            alloc.set_res(_emplace(alloc.resource(), value));
        }

        constexpr void push_front(T&& value)
        {
            alloc.set_res(_emplace(alloc.resource(), move(value)));
        }

        template<typename... Args>
        constexpr void emplace_front(Args&&... args)
        {
            alloc.set_res(_emplace(alloc.resource(), forward<Args>(args)...));
        }

        constexpr void pop_front()
        {
            snode* old_head = alloc.resource();
            snode* new_head = old_head->next;
            old_head->~snode();

            alloc.deallocate(old_head, 1);

            alloc.set_res(new_head);
        }

        constexpr void insert_after(iterator loc, const T& value)
        {
            snode* node = loc.get();
            node->next = _emplace(node->next, value);
        }

        constexpr void insert_after(iterator loc, T&& value)
        {
            snode* node = loc.get();
            node->next = _emplace(node->next, move(value));
        }

        template<typename... Args>
        constexpr void emplace_after(iterator loc, Args&&... args)
        {
            snode* node = loc.get();
            node->next = _emplace(node->next, forward<Args>(args)...);
        }

        constexpr void erase_after(iterator loc)
        {
            snode* node_to_delete = *(loc + 1).get();

            loc.get()->next = node_to_delete->next;
            alloc.deallocate(node_to_delete, 1);
        }

        constexpr ~slist() { clear(); }

        constexpr size_type size() const { return m_count; }

        constexpr void clear()
        {
            snode* head = alloc.resource();

            while(head)
            {
                snode* temp = head;
                head = head->next;
                temp.~snode();
            }

            m_count = 0;
            alloc.set_res(nullptr);
        }

        constexpr allocator_type* get_allocator() const noexcept { return &alloc; }

        constexpr iterator begin() { return _iterator{alloc.resource()}; }
        constexpr const_iterator begin() const { return _iterator{alloc.resource()}; }

        constexpr iterator end() { return _iterator{nullptr}; }
        constexpr const_iterator end() const { return _iterator{nullptr}; }
        
        [[nodiscard("This function does not empty the list.")]] constexpr bool empty()
        {
            return (begin() == end());
        }

    };

} // namespace aggro


#endif // LIST_HPP