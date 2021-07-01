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
        using s_node = snode<T>;

        struct _iterator
        {  
            using value_type = T;
            using size_type = std::size_t;
            
            s_node* node = nullptr;

            constexpr s_node* get() const { return node; }
            
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

            constexpr T& operator++() //prefix
            {
                node = node->next;
                return node->value;
            }

            constexpr T operator++(int) //postfix
            {
                T old = node->value;
                node = node->next;
                return old->value;
            };

            constexpr bool operator==(const _iterator& rhs)
            {
                if(this->node == rhs.node)
                {
                    return true;
                }
                return false;
            }

            constexpr bool operator!=(const _iterator& rhs)
            {
                if(this->node != rhs.node)
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
        constexpr s_node* _emplace(s_node* spot, Args&&... args)
        {
            s_node* new_node = alloc.allocate(1);
            new_node->next = spot;

            alloc.construct(new_node, forward<Args>(args)...);
            ++m_count;
            
            return new_node;
        }

    public:
        constexpr slist() = default;
        constexpr slist(const std::initializer_list<T>& init)
        {
            size_type init_size = init.size();
            s_node* current = nullptr;

            for(size_type i = 0; i < init_size; ++i)
            {
                if(alloc.resource() == nullptr)
                {
                    alloc.set_res(_emplace(nullptr, move(*(init.begin() + i))));
                    current = alloc.resource();
                }
                else
                {
                    current->next = _emplace(nullptr, move(*(init.begin() + i)));
                    current = current->next;
                }

            }
        }

        constexpr slist(const slist& other)
        {
            s_node* current = nullptr;
            
            for(auto val : other)
            {
                if(alloc.resource() == nullptr)
                {
                    alloc.set_res(_emplace(nullptr, val));
                    current = alloc.resource();
                }
                else
                {
                    current->next = _emplace(nullptr, val);
                    current = current->next;
                }

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
            s_node* old_head = alloc.resource();
            s_node* new_head = old_head->next;
            old_head->~s_node();

            alloc.deallocate(old_head, 1);

            alloc.set_res(new_head);
            --m_count;
        }

        constexpr void insert_after(iterator loc, const T& value)
        {
            if(loc.get() == nullptr) return;
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, value);
        }

        constexpr void insert_after(iterator loc, T&& value)
        {
            if(loc.get() == nullptr) return;
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, move(value));
        }

        template<typename... Args>
        constexpr void emplace_after(iterator loc, Args&&... args)
        {
            if(loc.get() == nullptr) return;
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, forward<Args>(args)...);
        }

        constexpr void erase_after(iterator loc)
        {
            if(loc.get() == nullptr) return;
            
            s_node* working_node = loc.get();
            s_node* node_to_delete = working_node->next;

            working_node->next = node_to_delete->next;

            node_to_delete->~s_node();

            alloc.deallocate(node_to_delete, 1);
            --m_count;
        }

        constexpr ~slist() { clear(); }

        constexpr size_type size() const { return m_count; }

        constexpr void clear()
        {
            s_node* head = alloc.resource();

            while(head)
            {
                s_node* temp = head;
                head = head->next;
                temp->~s_node();
                alloc.deallocate(temp, 1);
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


    template<os_compatible T>
    inline constexpr std::ostream& operator<<(std::ostream& os, const slist<T>& list)
    {
        os << "{ ";

        if(list.size() > 0)
        {
            bool first_item = true;

            for(auto& item : list)
            {
                if(first_item)
                    first_item = false;
                else
                    os << ", ";
                
                os << item;
            }
        }

        os << " }";

        return os;
    }

} // namespace aggro


#endif // LIST_HPP