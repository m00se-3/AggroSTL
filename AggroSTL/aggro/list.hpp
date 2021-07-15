#ifndef LIST_HPP
#define LIST_HPP

#include <initializer_list>
#include "utility.hpp"
#include "concepts/stream.hpp"
#include "allocators/standard.hpp"

namespace aggro
{
    
    /*
        Node struct for a slist.
    */
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

        //This iterator meets the 'LegacyForwardIterator' standard for forward_lists.
        template<typename T>
        struct s_iterator
        {  
            using value_type = T;
            using size_type = std::size_t;
            using s_node = snode<T>;
            
            s_node* node = nullptr;

            //Get the underlying pointer.
            constexpr s_node* get() const { return node; }
            
            constexpr T& operator*()
            {
                return node->value;
            }

            constexpr const T& operator*() const
            {
                return node->value;
            }

            constexpr s_iterator operator+(size_type index)
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
            }
        };

        template<typename T>
        inline constexpr bool operator==(const s_iterator<T>& lhs, const s_iterator<T>& rhs)
        {
            if(lhs.node == rhs.node)
            {
                return true;
            }
            return false;
        }

        template<typename T>
        inline constexpr bool operator!=(const s_iterator<T>& lhs, const s_iterator<T>& rhs)
        {
            if(lhs.node != rhs.node)
            {
                return true;
            }
            return false;
        }
    
    /*
        A singley-linked list used to replace std::forward_list. The default allocator takes an
        snode<T> as its template parameter.
    */
    template<typename T, standard_allocator Alloc = std_node_allocator<snode<T>>>
    class slist
    {   
        using s_node = snode<T>;

    public:

        using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = s_iterator<T>;
        using consts_iterator = const s_iterator<T>;
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
            o_all->unlink();
        }

        constexpr ~slist() { clear(); }

        //Return the first node.
        constexpr reference front() { return alloc.resource()->value; }

        //Return the first node.
        constexpr const_reference front() const { return alloc.resource()->value; }

        //Create a new node and make it the front node.
        constexpr iterator push_front(const T& value)
        {
            alloc.set_res(_emplace(alloc.resource(), value));
            return iterator{ alloc.resource() };
        }

        //Create a new node and make it the front node.
        constexpr iterator push_front(T&& value)
        {
            alloc.set_res(_emplace(alloc.resource(), move(value)));
            return iterator{ alloc.resource() };
        }

        //Construct a new node in place and make it the front node.
        template<typename... Args>
        constexpr iterator emplace_front(Args&&... args)
        {
            alloc.set_res(_emplace(alloc.resource(), forward<Args>(args)...));
            return iterator{ alloc.resource() };
        }

        //Remove the head node and make the second node the new front.
        constexpr void pop_front()
        {
            s_node* old_head = alloc.resource();
            s_node* new_head = old_head->next;
            old_head->~s_node();

            alloc.deallocate(old_head, 1);

            alloc.set_res(new_head);
            --m_count;
        }

        //Insert a value after the specified node location.
        constexpr iterator insert_after(iterator loc, const T& value)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, value);

            return iterator{ node->next };
        }

        //Insert a value after the specified node location.
        constexpr iterator insert_after(iterator loc, T&& value)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, move(value));

            return iterator{ node->next };
        }

        //Insert a new node at the specified location and construct a new object in place there.
        template<typename... Args>
        constexpr iterator emplace_after(iterator loc, Args&&... args)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            s_node* node = loc.get();
            node->next = _emplace(node->next, forward<Args>(args)...);

            return iterator{ node->next };
        }

        //Remove the specified node and preserve the link chain.
        constexpr void erase_after(iterator loc)
        {
            s_node* working_node = loc.get();
            
            if(working_node == nullptr) return;

            if(working_node->next)
            {
                s_node* node_to_delete = working_node->next;
                working_node->next = node_to_delete->next;

                node_to_delete->~s_node();

                alloc.deallocate(node_to_delete, 1);
                --m_count;
            }
        }


        //Get the number of nodes currently in the list.
        constexpr size_type size() const { return m_count; }

        //Remove all nodes from the list.
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

        //Get a pointer to the underlying allocator.
        constexpr allocator_type* get_allocator() const noexcept { return &alloc; }

        //Get an iterator to the start of the list.
        constexpr iterator begin() { return iterator{alloc.resource()}; }
        
        //Get an iterator to the start of the list.
        constexpr consts_iterator begin() const { return iterator{alloc.resource()}; }

        //Get an iterator to the location after the end of the list.
        constexpr iterator end() { return iterator{nullptr}; }
        
        //Get an iterator to the location after the end of the list.
        constexpr consts_iterator end() const { return iterator{nullptr}; }
        
        //Is the listy empty?
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


    /*
        Node struct for a dlist.
    */
    template<typename T>
    struct dnode
    {
        using value_type = T;
        
        T value {};
        dnode* prev = nullptr;
        dnode* next = nullptr;

        constexpr dnode() = default;
        constexpr dnode(const T& val) : value(val) {}
        constexpr dnode(T&& val) : value(move(val)) {}
        constexpr dnode(const T& val, dnode* p, dnode* n) : value(val), prev(p), next(n) {}
        constexpr dnode(T&& val, dnode* p, dnode* n) : value(move(val)), prev(p), next(n) {}
        constexpr ~dnode() = default;
    };


        //This iterator meets the 'LegacyForwardIterator' standard for forward_lists.
        template<typename T>
        struct d_iterator
        {  
            using value_type = T;
            using size_type = std::size_t;
            using d_node = dnode<T>;
            
            d_node* node = nullptr;

            //Get the underlying pointer.
            constexpr d_node* get() const { return node; }
            
            constexpr T& operator*()
            {
                return node->value;
            }

            constexpr const T& operator*() const
            {
                return node->value;
            }

            constexpr d_iterator operator+(size_type index)
            {
                while(this->node && index != 0)
                {
                    this->node = this->node->next;
                    --index;
                }

                return *this;
            }

            constexpr d_iterator operator-(size_type index)
            {
                while(this->node && index != 0)
                {
                    this->node = this->node->prev;
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
            }

            constexpr T& operator--() //prefix
            {
                node = node->prev;
                return node->value;
            }

            constexpr T operator--(int) //postfix
            {
                T old = node->value;
                node = node->prev;
                return old->value;
            }
        };

        template<typename T>
        inline constexpr bool operator==(const d_iterator<T>& lhs, const d_iterator<T>& rhs)
        {
            if(lhs.node == rhs.node)
            {
                return true;
            }
            return false;
        }

        template<typename T>
        inline constexpr bool operator!=(const d_iterator<T>& lhs, const d_iterator<T>& rhs)
        {
            if(lhs.node != rhs.node)
            {
                return true;
            }
            return false;
        }

    /*
        A doubley-linked list used to replace std::list. The default allocator takes an
        dnode<T> as its template parameter.
    */
    template<typename T, standard_allocator Alloc = std_node_allocator<dnode<T>>>
    class dlist
    {   
        using d_node = dnode<T>;

    public:

        using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = d_iterator<T>;
        using consts_iterator = const d_iterator<T>;
		using allocator_type = Alloc;

    private:

        allocator_type alloc;
        size_type m_count = 0;

        template<typename... Args>
        constexpr d_node* _emplace(d_node* spot, Args&&... args)
        {
            d_node* new_node = alloc.allocate(1);
            new_node->next = spot;
            if(spot)
            {
                new_node->prev = spot->prev;
            }
            else
            {
                d_node* end_node = alloc.resource_rev();
                if(end_node)
                {
                    new_node->prev = end_node;
                    end_node->next = new_node;
                }
            }

            alloc.construct(new_node, forward<Args>(args)...);
            ++m_count;
            
            return new_node;
        }

    public:

        constexpr dlist() = default;

        constexpr dlist(const std::initializer_list<T>& init)
        {
            size_type init_size = init.size();
            d_node* current = nullptr;

            for(size_type i = 0; i < init_size; ++i)
            {
                if(alloc.resource() == nullptr)
                {
                    d_node* node = _emplace(nullptr, move(*(init.begin() + i)));
                    alloc.set_res(node, node);
                    current = node;
                }
                else
                {
                    current->next = _emplace(nullptr, move(*(init.begin() + i)));
                    alloc.set_res(nullptr, current->next);
                    current = current->next;
                }

            }
        }

        constexpr dlist(const dlist& other)
        : m_count(other.size())
        {
            d_node* current = nullptr;
            
            for(auto val : other)
            {
                if(alloc.resource() == nullptr)
                {
                    d_node* node = _emplace(nullptr, val);
                    alloc.set_res(node, node);
                    current = node;
                }
                else
                {
                    current->next = _emplace(nullptr, val);
                    alloc.set_res(nullptr, current->next);
                    current = current->next;
                }

            }
        }

        constexpr dlist(dlist&& other)
        : m_count(other.size())
        {
            allocator_type* o_all = other.get_allocator();
            alloc.set_res(o_all->resource(), o_all->resource_rev());
            o_all->unlink();
        }

        constexpr ~dlist() { clear(); }

        //Return the first node.
        constexpr reference front() { return alloc.resource()->value; }

        //Return the first node.
        constexpr const_reference front() const { return alloc.resource()->value; }

        //Return the last node.
        constexpr reference back() { return alloc.resource_rev()->value; }

        //Return the last node.
        constexpr const_reference back() const { return alloc.resource_rev()->value; }

        //Create a new node and make it the front node.
        constexpr iterator push_front(const T& value)
        {
            alloc.set_res(_emplace(alloc.resource(), value));
            return iterator{ alloc.resource() };
        }

        //Create a new node and make it the front node.
        constexpr iterator push_front(T&& value)
        {
            alloc.set_res(_emplace(alloc.resource(), move(value)));
            return iterator{ alloc.resource() };
        }

        //Create a new node and make it the back node.
        constexpr iterator push_back(const T& value)
        {
            alloc.set_res(nullptr, _emplace(alloc.resource_rev(), value));
            return iterator{ alloc.resource_rev() };
        }

        //Create a new node and make it the back node.
        constexpr iterator push_back(T&& value)
        {
            alloc.set_res(nullptr, _emplace(nullptr, move(value)));
            return iterator{ alloc.resource_rev() };
        }

        //Construct a new node in place and make it the front node.
        template<typename... Args>
        constexpr iterator emplace_front(Args&&... args)
        {
            alloc.set_res(_emplace(alloc.resource(), forward<Args>(args)...));
            return iterator{ alloc.resource() };
        }

        //Construct a new node in place and make it the back node.
        template<typename... Args>
        constexpr iterator emplace_back(Args&&... args)
        {
            alloc.set_res(nullptr, _emplace(nullptr, forward<Args>(args)...));
            return iterator{ alloc.resource_rev() };
        }

        //Remove the head node and make the second node the new front.
        constexpr void pop_front()
        {
            d_node* old_head = alloc.resource();
            d_node* new_head = old_head->next;
            old_head->~d_node();

            alloc.deallocate(old_head, 1);

            alloc.set_res(new_head);
            --m_count;
        }

        //Remove the tail node and make the second node the new back.
        constexpr void pop_back()
        {
            d_node* old_tail = alloc.resource_rev();
            d_node* new_tail = old_tail->prev;
            old_tail->~d_node();

            alloc.deallocate(old_tail, 1);

            alloc.set_res(nullptr, new_tail);
            --m_count;
        }

        //Insert a value before the specified node location.
        constexpr iterator insert(iterator loc, const T& value)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            d_node* node = loc.get();
            d_node* new_node = _emplace(node, value);

            if(node->prev) node->prev->next = new_node;
            node->prev = new_node;

            return iterator{ node->prev };
        }

        //Insert a value before the specified node location.
        constexpr iterator insert(iterator loc, T&& value)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            d_node* node = loc.get();
            d_node* new_node = _emplace(node, value);

            if(node->prev) node->prev->next = new_node;
            node->prev = new_node;

            return iterator{ node->prev };
        }

        //Insert a new node at the specified location and construct a new object in place there.
        template<typename... Args>
        constexpr iterator emplace(iterator loc, Args&&... args)
        {
            if(loc.get() == nullptr) return iterator { nullptr };
            
            d_node* node = loc.get();
            d_node* new_node = _emplace(node, forward<Args>(args)...);

            if(node->prev) node->prev->next = new_node;
            node->prev = new_node;

            return iterator{ node->prev };
        }

        //Remove the specified node and preserve the link chain.
        constexpr void erase(iterator loc)
        {
            d_node* node_to_delete = loc.get();

            if(node_to_delete == nullptr) return;
            
            d_node* prev_node = node_to_delete->prev;

            if(prev_node)
            {
                prev_node->next = node_to_delete->next;

                if(prev_node->next) alloc.set_res(nullptr, prev_node);
            }
            else
            {
                alloc.set_res(node_to_delete->next);
            }

            node_to_delete->~d_node();

            alloc.deallocate(node_to_delete, 1);
            --m_count;
        }

        //Get the number of nodes currently in the list.
        constexpr size_type size() const { return m_count; }

        //Remove all nodes from the list.
        constexpr void clear()
        {
            d_node* head = alloc.resource();

            while(head)
            {
                d_node* temp = head;
                head = head->next;
                temp->~d_node();
                alloc.deallocate(temp, 1);
            }

            m_count = 0;
            alloc.set_res(nullptr);
        }

        //Get a pointer to the underlying allocator.
        constexpr allocator_type* get_allocator() const noexcept { return &alloc; }

        //Get an iterator to the start of the list.
        constexpr iterator begin() { return iterator{alloc.resource()}; }
        
        //Get an iterator to the start of the list.
        constexpr consts_iterator begin() const { return iterator{alloc.resource()}; }

        //Get an iterator to the location after the end of the list.
        constexpr iterator end() { return iterator{nullptr}; }
        
        //Get an iterator to the location after the end of the list.
        constexpr consts_iterator end() const { return iterator{nullptr}; }
        
        //Is the listy empty?
        [[nodiscard("This function does not empty the list.")]] constexpr bool empty()
        {
            return (begin() == end());
        }
    };

    template<os_compatible T>
    inline constexpr std::ostream& operator<<(std::ostream& os, const dlist<T>& list)
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