#ifndef AGGRO_DEQUE_HPP
#define AGGRO_DEQUE_HPP

#include "array.hpp"

namespace aggro
{
    //Node struct for the deque class.
    template<typename T, std::size_t Size>
    struct deque_node
    {
        using value_type = T;

        deque_node* prev = nullptr;
        deque_node* next = nullptr;
        array<T, Size> block;

        constexpr deque_node() = default;
        constexpr deque_node(deque_node* p, deque_node n) : prev(p), next(n), block() {}
        constexpr deque_node(std::initializer_list<T>&& inits, deque_node* p = nullptr, deque_node n = nullptr) : prev(p), next(n), block(move(inits)) {}

        constexpr array::iterator begin() { return block.begin(); }
        constexpr array::iterator end() { return block.end(); }

    };

    //Deque iterator.
    template<typename T, std::size_t Size>
    struct deque_iterator
    {
        using value_type = T;
        using size_type = std::size_t;
        using node = deque_node<T,Size>;

        node* it1 = nullptr;
        T* it2 = nullptr;

        deque_iterator() = default;
        deque_iterator(node* n) : it1(n), it2(n->begin()) {}
        deque_iterator(node* n, T* t) : it1(n), it2(t) {}

        //Get the underlying pointer.
        constexpr node* get() const { return this->it1; }

        constexpr deque_iterator& skip_front(size_type num)
        {
            size_type local_count = (this->it2 - this->it1->begin()) / sizeof(value_type);
            
            while(this->it1 && num != 0)
            {
                this->it1 = this->it1->next;
                --num;
            }

            if(!this->it1)
            {
                this->it2 = nullptr;
                return *this;
            }

            this->it2 = this->it1->begin() + local_count;

            return *this;
        }

        constexpr deque_iterator& skip_back(size_type num)
        {
            size_type local_count = (this->it2 - this->it1->begin()) / sizeof(value_type);
            
            while(this->it1 && num != 0)
            {
                this->it1 = this->it1->prev;
                --num;
            }

            if(!this->it1)
            {
                this->it2 = nullptr;
                return *this;
            }

            this->it2 = this->it1->begin() + local_count;

            return *this;
        }
        
        constexpr T& operator*()
        {
            return *this->it2;
        }

        constexpr const T& operator*() const
        {
            return *this->it2;
        }

        constexpr deque_iterator operator+(size_type index)
        {
            const size_type hops = index / (Size - 1u);
            size_type steps = index % (Size - 1u);

            if(hops > 0) skip_front(hops);

            while(this->it2 && steps != 0u)
            {
                this->it2++;
                if(this->it2 == this->it1->end())
                {
                    this->it1 = this->it1->next;
                    if(this->it1)
                        this->it2 = this->it1->begin();
                    else
                    {
                        this->it2 = nullptr;
                        break;
                    }

                    --steps;
                }
            }

            return *this;
        }

        constexpr deque_iterator operator-(size_type index)
        {
            const size_type hops = index / (Size - 1u);
            size_type steps = index % (Size - 1u);

            if(hops > 0) skip_back(hops);

            while(this->it2 && steps != 0u)
            {
                if(this->it2 == this->it1->begin())
                {
                    this->it1 = this->it1->prev;
                    if(this->it1)
                        this->it2 = this->it1->end() - 1u;
                    else
                    {
                        this->it2 = nullptr;
                        break;
                    }

                    --steps;
                }
                this->it2--;
            }

            return *this;
        }

        constexpr deque_iterator& operator++() //prefix
        {
            this->it2++;
            if(this->it2 == it1->end())
            {
                this->it1 = it1->next;
                if(this->it1)
                    this->it2 = it1->begin();
                else
                    this->it2 = nullptr;
            }

            return *this;
        }

        constexpr deque_iterator& operator++(int) //postfix
        {
            deque_iterator temp = *this;
            
            this->it2++;
            if(this->it2 == this->it1->end())
            {
                this->it1 = this->it1->next;
                if(this->it1)
                    this->it2 = it1->begin();
                else
                    this->it2 = nullptr;
            }

            return temp;
        }

        constexpr deque_iterator& operator--() //prefix
        {
            if(this->it2 == this->it1->begin())
            {
                this->it1 = this->it1->prev;
                if(this->it1)
                    this->it2 = it1->end() - 1u;
                else
                    this->it2 = nullptr;
            }
            else
            {
                this->it2--;
            }

            return *this;
        }

        constexpr deque_iterator& operator--(int) //postfix
        {
            deque_iterator temp = *this;

            if(this->it2 == it1->begin())
            {
                this->it1 = this->it1->prev;
                if(this->it1)
                    this->it2 = it1->end() - 1u;
                else
                    this->it2 = nullptr;
            }
            else
            {
                this->it2--;
            }

            return temp;
        }
    };

    template<typename T, std::size_t Size>
    inline constexpr bool operator==(const deque_iterator<T,Size>& lhs, const deque_iterator<T,Size>& rhs)
    {
        if(lhs.it1 == rhs.it1 && lhs.it2 == lhs.it2)
        {
            return true;
        }
        return false;
    }

    template<typename T, std::size_t Size>
    inline constexpr bool operator!=(const deque_iterator<T,Size>& lhs, const deque_iterator<T,Size>& rhs)
    {
        if(lhs.it1 != rhs.it1 || lhs.it2 != lhs.it2)
        {
            return true;
        }
        return false;
    }

    template<typename T, std::size_t Size, standard_allocator Alloc = std_node_allocator<deque_node<T,Size>>>
    class deque
    {   
        using node = deque_node<T,Size>;

    public:

        using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = deque_iterator<T,Size>;
        using const_iterator = const deque_iterator<T,Size>;
		using allocator_type = Alloc;

    private:

        allocator_type alloc;
        size_type m_count = 0u;

        template<typename... Args>
        constexpr void _emplace(iterator spot, Args&&... args)
        {
            alloc.construct(spot->it2, forward<Args>(args)...);
            ++m_count;
        }

        constexpr iterator create_node(node* p = nullptr, node* n = nullptr)
        {
            node* new_node = alloc.allocate(1);

            if(p)
                new_node->prev = p;
            else
                alloc.set_res(new_node);

            if(n)
                new_node->next = n;
            else
                alloc.set_res(nullptr, new_node);

            return iterator{ new_node, new_node->begin() };
        }

    public:

        constexpr deque() = default;
        constexpr ~deque() = { clear(); }





        constexpr void clear()
        {
            node* b = begin().get();

            while(b)
            {
                for(auto& item : *(b->block))
                {
                    item.~T();
                }

                node* temp = b;
                b = b->next;
                alloc.deallocate(temp, 1u);
            }

            m_count = 0u;
        }

        constexpr iterator begin() { return alloc.resource(); }
        constexpr const_iterator begin() const { alloc.resource(); }

        constexpr iterator end()
        {
            node* en = alloc.resource_rev();

            if(en)
                return iterator{ en, en.end() };
            else
                return iterator{};
        }

        constexpr const_iterator end() const
        {
            node* en = alloc.resource_rev();

            if(en)
                return const_iterator{ en, en.end() };
            else
                return const_iterator{};
        }
    };

} // namespace aggro


#endif // AGGRO_DEQUE_HPP