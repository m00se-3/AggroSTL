#include <initializer_list>
#include "utility.hpp"
#include "optional.hpp"
#include "concepts/stream.hpp"
#include "allocators/standard.hpp"

namespace aggro
{
	enum class ArrayExpandMethod : uint8_t
	{
		INCREMENT = 0, PLUS_HALF, DOUBLE
	};

	//Stack allocated array which supports iterators. Replaces std::array.
	template<typename T, std::size_t N>
	class array
	{
	
	private:
		T m_data[N];

	public:
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = T*;
		using const_reverse_iterator = const T*;

		constexpr array() = default;
		constexpr array(const array& other)
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];
		}

		constexpr array(array&& other) noexcept
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];
		}

		constexpr array(std::initializer_list<T>&& inits)
		{
			for (size_type n = 0; n < size(); n++)
				new(&m_data[n]) T(move(*(inits.begin() + n)));
		}
		constexpr ~array() = default;

		constexpr array& operator=(const array& other)
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];

			return *this;
		}

		constexpr array& operator=(array&& other) noexcept
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];

			return *this;
		}

		constexpr array& operator=(std::initializer_list<T>&& inits)
		{
			for (size_type n = 0; n < size(); n++)
				new(&m_data[n]) T(move(*(inits.begin() + n)));

			return *this;
		}

		constexpr T& operator[](size_type index) 
		{
			return m_data[index]; 
		}

		const T& operator[](size_type index) const 
		{
			return m_data[index]; 
		}

		constexpr size_type size() const { return N; }
		constexpr size_type bytes() const { return N * sizeof(T); }

		constexpr T* data() { return m_data; }

		//Returns an optional value at 'index' location.
		//Warning: This operation will copy the object!
		constexpr aggro::optional<T> at(size_type index)
		{
			if (index < N)
				return m_data[index];
			else
				return aggro::nullopt;
		}

		[[nodiscard("This function does not empty the array.")]] constexpr bool empty() const { return begin() == end(); }

		constexpr iterator begin() { return m_data; }
		constexpr iterator end() { return m_data + N; }

		constexpr const_iterator begin() const { return m_data; }
		constexpr const_iterator end() const { return m_data + N; }

		constexpr reverse_iterator rbegin() { return m_data + N; }
		constexpr reverse_iterator rend() { return m_data; }

		constexpr const_reverse_iterator rbegin() const { return m_data + N; }
		constexpr const_reverse_iterator rend() const { return m_data; }

		
	};

	/*
		Dynamically allocated array which replaces std::vector. By default dynarrs grow exponentially, reducing calls to new.
		The method used to resize the array upon adding a new element can be changed using the ArrayExpandMethod enum.
	*/
	template<typename T, standard_allocator Alloc = std_contiguous_allocator<T>>
	class dynarr
	{
	public:
		using size_type = std::size_t;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = T*;
		using const_reverse_iterator = const T*;
		using allocator_type = Alloc;

	private:

		allocator_type alloc;
		size_type m_count {};
		size_type m_capacity {};

		template<typename... Args>
		constexpr void _emplace(T* spot, Args&&... args)
		{
			alloc.construct(spot, forward<Args>(args)...);
		}

		constexpr void grow()
		{
			T* temp = alloc.resource();
			size_type nCap, oCap;

			auto decide = [](size_type test) -> size_type {
				if (test > 0)
					return test;
				else
					return 1;
			};

			switch (expand_method)
			{
			case ArrayExpandMethod::INCREMENT:
				nCap = decide(m_capacity + 1);
				break;
			case ArrayExpandMethod::PLUS_HALF:
				nCap = decide(m_capacity + (m_capacity / 2));
				break;
			case ArrayExpandMethod::DOUBLE:
				nCap = decide(m_capacity * 2);
				break;
			default:
				nCap = decide(m_capacity + 1);
				break;
			}

			alloc.set_res(alloc.allocate(nCap));
			oCap = m_capacity;
			m_capacity = nCap;

			for (size_type i = 0; i < m_count; i++)
			{
				_emplace(&alloc.resource()[i], move(temp[i]));
				temp[i].~T();
			}

			alloc.deallocate(temp, oCap);
		}

	public:

		friend constexpr void nullify_array(const dynarr& arr);

		ArrayExpandMethod expand_method = ArrayExpandMethod::PLUS_HALF;

		constexpr dynarr() = default;

		//Allocates a buffer of 'cap' size.
		//Does not construct any objects.
		constexpr dynarr(size_type cap)
			:m_capacity(cap)
		{
			alloc.set_res(alloc.allocate(cap));
		}

		//Allocates a buffer of'inits.size()' size and moves the provided objects into it.
		constexpr dynarr(std::initializer_list<T>&& inits)
			:m_count(0), m_capacity(inits.size())
		{
			alloc.set_res(alloc.allocate(m_capacity));
			for (size_type i = 0; i < m_capacity; i++)
			{
				_emplace(&alloc.resource()[i], move(*(inits.begin() + i)));
				++m_count;
			}
		}

		constexpr dynarr(const dynarr& other)
			:m_count(other.size()), m_capacity(other.capacity())
		{
			alloc.set_res(alloc.allocate(m_capacity));

			if (alloc.resource() != nullptr)
			{
				for (size_type i = 0; i < m_count; i++)
				{
					_emplace(&alloc.resource()[i], other.data()[i]);
				}
			}
		}

		constexpr dynarr(dynarr&& other) noexcept
			:m_count(other.size()), m_capacity(other.capacity())
		{
			alloc.set_res(other.data());
			nullify_array(other);
		}

		constexpr T& operator[](size_type index)
		{
			return alloc.resource()[index];
		}

		constexpr const T& operator[](size_type index) const
		{
			return alloc.resource()[index];
		}

		constexpr dynarr& operator=(const dynarr& other)
		{
			if (this != &other)
			{
				clear();
				alloc.deallocate(alloc.resource(), m_capacity);


				m_count = other.size();
				m_capacity = other.capacity();

				alloc.set_res(alloc.allocate(m_capacity));

				if (alloc.resource() != nullptr)
				{
					for (size_type i = 0; i < m_count; i++)
					{
						_emplace(&alloc.resource()[i], other.data()[i]);
					}
				}
			}

			return *this;
		}

		constexpr dynarr& operator=(dynarr&& other) noexcept 
		{
			if (this != &other)
			{
				clear();
				alloc.deallocate(alloc.resource(), m_capacity);

				m_count = other.size();
				m_capacity = other.capacity();

				alloc.set_res(other.data());

				nullify_array(other);
			}

			return *this;
		}

		constexpr dynarr& operator=(std::initializer_list<T>&& list)
		{
			clear();
			
			if(list.size() > capacity())
			{
				alloc.deallocate(alloc.resource(), m_capacity);
				m_capacity = list.size();
				alloc.set_res(alloc.allocate(m_capacity));
			}

			for (size_type i = 0; i < m_capacity; i++)
			{
				_emplace(&alloc.resource()[i], move(*(list.begin() + i)));
				++m_count;
			}

			return *this;
		}

		constexpr ~dynarr()
		{
			clear();
			alloc.deallocate(alloc.resource(), m_capacity);
			m_capacity = 0;
		}

		//Returns an optional value at 'index' location.
		//Warning: This operation will copy the object!
		constexpr aggro::optional<T> at(size_type index) const
		{
			if (index < m_count)
				return make_optional<T>(alloc.resource()[index]);
			else
				return aggro::nullopt;
		}

		//Return the first element in the array.
		constexpr T& front() const { return alloc.resource()[0]; }

		//Return the last element in the array.
		constexpr T& back() const { return alloc.resource()[m_count - 1]; }

		//Number of elements contained.
		constexpr size_type size() const
		{
			return m_count;
		}

		//Number of elements able to be held.
		constexpr size_type capacity() const
		{
			return m_capacity;
		}

		//Total size in bytes
		constexpr size_t bytes() const
		{
			return m_count * sizeof(T);
		}

		//Return raw pointer to the array data.
		constexpr const T* data() const { return alloc.resource(); }
		constexpr T* data() { return alloc.resource(); }

		constexpr const_iterator begin() const { return alloc.resource(); }
		constexpr iterator begin() { return alloc.resource(); }

		constexpr const_iterator end() const { return alloc.resource() + m_count; }
		constexpr iterator end() { return alloc.resource() + m_count; }

		constexpr reverse_iterator rbegin() { return alloc.resource() + m_count; }
		constexpr reverse_iterator rend() { return alloc.resource() - 1; }

		constexpr const_reverse_iterator rbegin() const { return alloc.resource() + m_count; }
		constexpr const_reverse_iterator rend() const { return alloc.resource() - 1; }

		//Is the array empty?
		[[nodiscard("This function does not empty the array.")]] constexpr bool empty() const { return begin() == end(); }

		//Reallocate enough memory for the provided number of elements.
		constexpr void reserve(size_type cap)
		{
			T* temp = alloc.resource();

			alloc.set_res(alloc.allocate(cap));

			for (size_type i = 0; i < m_count; i++)
			{
				_emplace(&alloc.resource()[i], move(temp[i]));
				temp[i].~T();
			}

			alloc.deallocate(temp, m_capacity);

			m_capacity = cap;
		}

		//Store a copy of the provided object in the array.
		constexpr void push_back(const T& element)
		{
			emplace_back(element);
		}

		//Move the provided object into the array.
		constexpr void push_back(T&& element)
		{
			emplace_back(move(element));
		}

		//Construct an object of type T directly into the array using the
		//provided arguments.
		template<typename... Args>
		constexpr void emplace_back(Args&&... args)
		{
			if (m_count >=m_capacity)
				grow();

			_emplace(&alloc.resource()[m_count], forward<Args>(args)...);

			++m_count;
		}

		//Erase the last element. Similar to pop_back.
		constexpr void pop_back()
		{
			if (m_count > 0) --m_count;
			
			alloc.resource()[m_count].~T();
		}

		//Erase all elements from the starting point to the stopping point.
		//If start is nullptr, this will start at the begining of the array.
		//If stop is nullptr, this will stop at the end of the array.
		constexpr void erase(T* start, T* stop = nullptr)
		{
			if (start == end()) return;
			if (stop == nullptr) stop = end();
			if (start == nullptr) start = begin();

			while (start != stop)
			{
				start->~T();
				m_count--;
				start++;
			}
		}

		//clears the whole array and resets the pushm_counter.
		constexpr void clear()
		{
			for (size_type i = 0; i < m_count; i++)
				alloc.resource()[i].~T();

			m_count = 0;
		}
	};

	//Used for move operations.
	template<typename T>
	constexpr void nullify_array(const dynarr<T>& arr)
	{
		arr.count = 0;
		arr.capacity = 0;
		arr.alloc.set_res(nullptr);
	}

	template<os_compatible T, size_t N>
	inline constexpr std::ostream& operator<<(std::ostream& stream, const array<T, N>& obj)
	{
		stream << "{ ";

		for(size_t ind = 0; ind < N - 1; ++ind)
			stream << obj[ind] << ", ";
		
		stream << obj[N-1] << " }";

		return stream;
	}

	template<os_compatible T>
	inline constexpr std::ostream& operator<<(std::ostream& stream, const dynarr<T>& obj)
	{
		stream << "{ ";

		if(obj.size() > 0)
		{
			auto size = obj.size() - 1;

			for(size_t ind = 0; ind < size; ++ind)
				stream << obj[ind] << ", ";
			
			stream << obj[size];
		}
			stream  << " }";

		return stream;
	}
}
