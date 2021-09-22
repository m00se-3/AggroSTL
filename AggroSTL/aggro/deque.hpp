#ifndef AGGRO_DEQUE_HPP
#define AGGRO_DEQUE_HPP

#include "list.hpp"
#include "array.hpp"

namespace aggro
{
    template<typename T, std::size_t Size, standard_allocator Alloc>
    struct deque_iterator
    {
        using value_type = T;
        using size_type = std::size_t;
        using node = dlist<array<T, Size>, Alloc>::d_node;

        node* book = nullptr;
        size_type index = 0u;

        deque_iterator(node* bk) : book(bk) {}
        deque_iterator(node* bk, size_type i) : book(bk), index(i) {}
        deque_iterator(const deque_iterator& other) : book(other.book), index(other.index)

        //Get the underlying pointer.
        constexpr node* get() const { return book; }

        constexpr T& operator*()
        {
            return book->value[index];
        }

        constexpr const T& operator*() const
        {
            return book->value[index];
        }

        constexpr deque_iterator operator+(size_type ind)
        {
            while (book && ind > Size - 1u)
            {
                book = book->next;
                ind -= Size;
            }

            if (book) index = ind;

            return *this;
        }

        constexpr deque_iterator operator-(size_type ind)
        {
            while (book && ind > Size - 1u)
            {
                book = book->prev;
                ind -= Size;
            }

            if (book) index = Size - ind;

            return *this;
        }

        constexpr deque_iterator& operator+=(size_type ind)
        {
            while (book && ind > Size - 1u)
            {
                book = book->next;
                ind -= Size;
            }

            if (book) index = ind;

            return *this;
        }

        constexpr deque_iterator& operator-=(size_type ind)
        {
            while (book && ind > Size - 1u)
            {
                book = book->prev;
                ind -= Size;
            }

            if (book) index = Size - ind;

            return *this;
        }

        constexpr deque_iterator& operator++() //prefix
        {
            ++index;
            if (index == Size)
            {
                book = book->next;
                index = 0u;
            }

            return *this;
        }

        constexpr deque_iterator& operator++(int) //postfix
        {
            deque_iterator temp = *this;
            
            ++index;
            if (index == Size)
            {
                book = book->next;
                index = 0u;
            }

            return temp;
        }

        constexpr deque_iterator& operator--() //prefix
        {
            --index;
            if (index == 0u)
            {
                book = book->prev;
                index = Size - 1u;;
            }

            return *this;
        }

        constexpr deque_iterator& operator--(int) //postfix
        {
            deque_iterator temp = *this;
            
            --index;
            if (index == 0u)
            {
                book = book->prev;
                index = Size - 1u;;
            }

            return temp;
        }
    };

    template<typename T, std::size_t Size, standard_allocator Alloc>
    inline constexpr bool operator==(const deque_iterator<T, Size, Alloc>& lhs, const deque_iterator<T, Size, Alloc>& rhs)
    {
        if (lhs.book == rhs.book && lhs.index == rhs.index)
        {
            return true;
        }
        return false;
    }

    template<typename T, std::size_t Size, standard_allocator Alloc>
    inline constexpr bool operator!=(const deque_iterator<T, Size, Alloc>& lhs, const deque_iterator<T, Size, Alloc>& rhs)
    {
        if (lhs.book != rhs.book || lhs.index != rhs.index)
        {
            return true;
        }
        return false;
    }
    
    template<typename T, std::size_t Size, standard_allocator Alloc = std_node_allocator<T>>
    class deque
    {   
    public:

        using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = deque_iterator<T, Size, Alloc>;
        using const_iterator = const deque_iterator<T, Size, Alloc>;
        using reverse_iterator = deque_iterator<T, Size, Alloc>;
        using const_reverse_iterator = const deque_iterator<T, Size, Alloc>;

        using allocator_type = Alloc;

    private:

        dlist<array<T, Size>, Alloc> ledger;
        size_type m_count = 0u;
        size_type offset = 0u;

        template<typename... Args>
        constexpr void _emplace(pointer spot, Args&&... args)
        {
            ledger.get_allocator()->construct(spot, forward<Args>(args)...);
            ++m_count;
        }

        friend static size_type get_offset(const deque& o) { return o.offset; }
        friend static const dlist<array<T, Size>>& get_ledger(const deque& o) { return o.ledger; }
        friend static dlist<array<T, Size>>&& get_ledger(deque&& o) { return forward<dlist<array>T, Size>&&(o.ledger); }

    public:

        constexpr deque() = default;
        constexpr ~deque() { clear(); }

        constexpr deque(const deque& other)
            : ledger(get_ledger(other)), m_count(other.size()), offset(get_offset(other)) 
        {}

        constexpr deque(deque&& other) noexcept
            : ledger(move(get_ledger(move(other)))), m_count(other.size()), offset(get_offset(other))
        {}

        constexpr reference operator[](size_type index)
        {
            index += offset;
            if (index < Size)
            {
                return ledger.front()[index];
            }
            else
            {
                const size_type jumps = index / Size;
                const size_type steps = index % Size;

                return *((ledger.begin() + jumps).data() + steps);
            }
        }

        constexpr const_reference operator[](size_type index) const
        {
            index += offset;
            if (index < Size)
            {
                return ledger.front()[index];
            }
            else
            {
                const size_type jumps = index / Size;
                const size_type steps = index % Size;

                return *((ledger.begin() + jumps).data() + steps);
            }
        }

        constexpr aggro::optional_ref<T> at(size_type index)
        {            
            if(index < m_count)
            {
                index += offset;
                if (index < Size)
                {
                    return ledger.front()[index];
                }
                else
                {
                    const size_type jumps = index / Size;
                    const size_type steps = index % Size;

                    return *((ledger.begin() + jumps).data() + steps);
                }
            }
            else
            {
                return aggro::nullopt_ref_t{};
            }
        }

        constexpr const aggro::optional_ref<T> at(size_type index) const
        {
            if (index < m_count)
            {
                index += offset;
                if (index < Size)
                {
                    return ledger.front()[index];
                }
                else
                {
                    const size_type jumps = index / Size;
                    const size_type steps = index % Size;

                    return *((ledger.begin() + jumps).data() + steps);
                }
            }
            else
            {
                return aggro::nullopt_ref_t{};
            }
        }

        template<typename... Args>
        constexpr iterator emplace_front(Args&&... args)
        {
            if (offset == 0u)
            {
                ledger.emplace_front();
                offset = Size - 1u;
                _emplace(&(ledger.front()[offset]), forward<Args>(args)...);
            }
            else
            {
                --offset;
                _emplace(&(ledger.front()[offset]), forward<Args>(args)...);
            }

            return iterator{ ledger.begin().get(), offset };
        }

        constexpr iterator push_front(const T& elem) { return emplace_front(elem); }
        constexpr iterator push_front(T&& elem) { return emplace_front(move(elem)); }

        template<typename... Args>
        constexpr iterator emplace_back(Args&&... args)
        {
            if (empty()) return emplace_front(forward<Args>(args)...);

            const size_type loc = (offset + m_count) % Size;

            if (loc == 0u) ledger.emplace_back();
            _emplace((ledger.back()[loc]), forward<Args>(args)...);

            return iterator{ get_allocator()->resource_rev(), loc };
        }

        constexpr iterator push_back(const T& elem) { return emplace_back(elem); }
        constexpr iterator push_back(T&& elem) { return emplace_back(move(elem)); }

        constexpr void pop_front()
        {
            if (empty()) return;

            if (offset == Size - 1u)
            {
                offset = 0u;
                ledger.pop_front();
            }
            else
            {
                ledger.front()[offset].~T();
                ++offset;
            }

            --m_count;
        }

        constexpr void pop_back()
        {
            if (empty()) return;

            const size_type loc = (offset + m_count) % Size;

            if (loc == 0u)
            {
                ledger.pop_back();
            }
            else
            {
                ledger.back()[loc].~T();
            }

            --m_count;
        }

        constexpr allocator_type* get_allocator() const noexcept { return ledger.get_allocator(); }
        constexpr size_type size() const { return m_count; }

        [[nodiscard("Function does not empty the container.")]] constexpr bool empty() const { return begin() == end(); }

        constexpr void clear()
        {
            ledger.clear();
            offset = 0u;
            m_count = 0u;
        }

        constexpr reference front() { return ledger.front()[offset]; }
        constexpr const_reference front() const { return ledger.front()[offset]; }

        constexpr reference back() { return ledger.back()[(m_count - offset) % Size]; }
        constexpr const_reference back() const { return ledger.back()[(m_count - offset) % Size]; }
        
        constexpr iterator begin() { return iterator{ ledger.begin()) }; }
        constexpr const_iterator begin() const { return const_iterator{ ledger.begin() }; }

        constexpr iterator end() { return iterator{ nullptr }; }
        constexpr const_iterator end() const { return const_iterator{ nullptr }; }
    };

} // namespace aggro


#endif // AGGRO_DEQUE_HPP