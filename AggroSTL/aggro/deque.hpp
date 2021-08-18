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

        aggro::array<T, Size> block;
        deque_node* prev = nullptr;
        deque_node* next = nullptr;

        constexpr deque_node() = default;
        constexpr deque_node(deque_node* p, deque_node n) : block(), prev(p), next(n) {}
        constexpr deque_node(std::initializer_list<T>&& inits, deque_node* p = nullptr, deque_node n = nullptr) : block(aggro::move(inits)), prev(p), next(n) {}
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
        deque_iterator(node* n) : it1(n), it2(n->block->begin()) {}
        deque_iterator(node* n, T* t) : it1(n), it2(t) {}

        //Get the underlying pointer.
        constexpr node* get() const { return this->it1; }
        
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
            while(this->it1 && index != 0u)
            {
                while(this->it2 && index != 0u)
                {
                    this->it2++;
                    if(this->it2 == this->it1->block->end()) break;
                    --index;
                }

                if(index != 0u)
                {
                    this->it1 = this->it1->next;
                    if(!this->it1) return deque_iterator(nullptr, nullptr);
                    this->it2 = this->it1->block->begin();
                    --index;
                }
            }

            return *this->it2;
        }

        constexpr deque_iterator operator-(size_type index)
        {
            while(this->it1 && index != 0u)
            {
                while(this->it2 && index != 0u)
                {
                    if(this->it2 == this->it1->block->begin()) break;
                    this->it2--;
                    --index;
                }

                if(index != 0u)
                {
                    this->it1 = this->it1->prev;
                    if(!this->it1) return deque_iterator(nullptr, nullptr);
                    this->it2 = this->it1->block->end() - 1;
                    --index;
                }
            }

            return *this->it2;
        }

        constexpr T& operator++() //prefix
        {
            this->it2++;
            if(this->it2 == it1->block->end())
            {
                this->it1 = it1->next;
                this->it2 = it1->block->begin();
            }

            return *it2;
        }

        constexpr T& operator++(int) //postfix
        {
            T* temp = this->it2;
            
            this->it2++;
            if(this->it2 == this->it1->block->end())
            {
                this->it1 = this->it1->next;
                this->it2 = this->it1->block->begin();
            }

            return *temp;
        }

        constexpr T& operator--() //prefix
        {
            if(this->it2 == this->it1->block->begin())
            {
                this->it1 = this->it1->prev;
                this->it2 = this->it1->block->end() - 1;
            }
            else
            {
                this->it2--;
            }

            return *this->it2;
        }

        constexpr T& operator--(int) //postfix
        {
            T* temp = this->it2;

            if(this->it2 == it1->block->begin())
            {
                this->it1 = this->it1->prev;
                this->it2 = this->it1->block->end() - 1;
            }
            else
            {
                this->it2--;
            }

            return *temp;
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

} // namespace aggro


#endif // AGGRO_DEQUE_HPP