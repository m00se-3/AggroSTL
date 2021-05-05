#include <utility>
#include <initializer_list>
#include <optional>
#include "concepts/StreamConcepts.hpp"
#include "allocators/STDAllocator.hpp"

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
		using value_type = T;
		using iterator = T*;
		using constiterator = const T*;
		using reviterator = T*;
		using constreviterator = const T*;

		T m_data[N];

	public:
		using size_type = std::size_t;

		array() = default;
		array(const array& other)
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];
		}

		array(array&& other) noexcept
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];
		}

		array(std::initializer_list<T>&& inits)
		{
			for (size_type n = 0; n < size(); n++)
				new(&m_data[n]) T(std::move(*(inits.begin() + n)));
		}
		~array() = default;

		array& operator=(const array& other)
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];

			return *this;
		}

		array& operator=(array&& other) noexcept
		{
			for (size_type n = 0; n < size(); n++)
				m_data[n] = other[n];

			return *this;
		}

		array& operator=(std::initializer_list<T>&& inits)
		{
			for (size_type n = 0; n < size(); n++)
				new(&m_data[n]) T(std::move(*(inits.begin() + n)));

			return *this;
		}

		T& operator[](size_type index) 
		{
			return m_data[index]; 
		}

		const T& operator[](size_type index) const 
		{
			return m_data[index]; 
		}

		constexpr size_type size() const { return N; }
		constexpr size_type bytes() const { return N * sizeof(T); }

		T* data() { return m_data; }

		std::optional<T> At(size_type index)
		{
			if (index < N)
				return m_data[index];
			else
				return std::nullopt;
		}

		[[nodiscard("This function does not empty the array.")]] bool empty() const { return begin() == end(); }

		iterator begin() { return m_data; }
		iterator end() { return m_data + N; }

		constiterator begin() const { return m_data; }
		constiterator end() const { return m_data + N; }

		reviterator rbegin() { return m_data + N; }
		reviterator rend() { return m_data; }

		constreviterator rbegin() const { return m_data + N; }
		constreviterator rend() const { return m_data; }

		
	};

	/*
		Dynamically allocated array which replaces std::vector. By default the dynarrs grow exponentially, reducing calls to new.
		The method used to resize the array upon adding a new element can be changed using the ArrayExpandMethod enum.
	*/
	template<typename T, standard_allocator Alloc = std_allocator<T>>
	class dynarr
	{
	public:
		using size_type = std::size_t;

	private:

		using iterator = T*;
		using constiterator = const T*;
		using reviterator = T*;
		using constreviterator = const T*;
		using resource = Alloc;

		resource alloc;
		size_type m_count {};
		size_type m_capacity {};

		void grow()
		{
			T* temp = alloc.m_buffer;
			size_type nCap;
			size_type oCap = m_capacity;

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
			}

			alloc.allocate(nCap);
			m_capacity = nCap;

			for (size_type i = 0; i < m_count; i++)
			{
				new(&alloc.m_buffer[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			alloc.deallocate(temp, oCap);
		}

	public:

		friend void nullify_array(const dynarr& arr);

		ArrayExpandMethod expand_method = ArrayExpandMethod::PLUS_HALF;

		dynarr() = default;

		dynarr(size_type cap)
			:m_capacity(cap)
		{
			alloc.allocate(cap);
		}

		dynarr(std::initializer_list<T>&& inits)
			:m_count(0), m_capacity(inits.size())
		{
			alloc.allocate(m_capacity);
			for (size_type i = 0; i < m_capacity; i++)
			{
				new(&alloc.m_buffer[i]) T(std::move(*(inits.begin() + i)));
				++m_count;
			}
		}

		dynarr(const dynarr& other)
			:m_count(other.size()), m_capacity(other.capacity())
		{
			alloc.allocate(m_capacity);

			if (alloc.m_buffer != nullptr)
			{
				for (size_type i = 0; i < m_count; i++)
				{
					new(&alloc.m_buffer[i]) T(other.data()[i]);
				}
			}
		}

		dynarr(dynarr&& other) noexcept
			:m_count(other.size()), m_capacity(other.capacity())
		{
			alloc.m_buffer = other.data();
			NullifyArray(other);
		}

		T& operator[](size_type index)
		{
			return alloc.m_buffer[index];
		}

		const T& operator[](size_type index) const
		{
			return alloc.m_buffer[index];
		}

		dynarr& operator=(const dynarr& other)
		{
			if (this != &other)
			{
				clear();
				alloc.deallocate(m_capacity);


				m_count = other.size();
				m_capacity = other.capacity();

				alloc.allocate(m_capacity);

				if (alloc.m_buffer != nullptr)
				{
					for (size_type i = 0; i < m_count; i++)
					{
						new(&alloc.m_buffer[i]) T(other.data()[i]);
					}
				}
			}

			return *this;
		}

		dynarr& operator=(dynarr&& other) noexcept 
		{
			if (this != &other)
			{
				clear();
				alloc.deallocate(m_capacity);

				m_count = other.size();
				m_capacity = other.capacity();

				alloc.m_buffer = other.data();

				NullifyArray(other);
			}

			return *this;
		}

		dynarr& operator=(std::initializer_list<T>&& list)
		{
			clear();
			
			if(list.size() > capacity())
			{
				alloc.deallocate(m_capacity);
				m_capacity = list.size();
				alloc.allocate(m_capacity);
			}

			for (size_type i = 0; i < m_capacity; i++)
			{
				new(&alloc.m_buffer[i]) T(std::move(*(list.begin() + i)));
				++m_count;
			}

			return *this;
		}

		~dynarr()
		{
			clear();
			alloc.deallocate(m_capacity);
			m_capacity = 0;
		}

		std::optional<T> At(size_type index)
		{
			if (index < m_count)
				return alloc.m_buffer[index];
			else
				return std::nullopt;
		}

		//Return the first element in the array.
		T& front() const { return alloc.m_buffer[0]; }

		//Return the last element in the array.
		T& back() const { return alloc.m_buffer[m_count - 1]; }

		//Number of elements contained.
		size_type size() const
		{
			return m_count;
		}

		//Number of elements able to be held.
		size_type capacity() const
		{
			return m_capacity;
		}

		//Total size in bytes
		size_t bytes() const
		{
			return m_count * sizeof(T);
		}

		//Return raw pointer to the array data.
		const T* data() const { return alloc.m_buffer; }
		T* data() { return alloc.m_buffer; }

		constiterator begin() const { return alloc.m_buffer; }
		iterator begin() { return alloc.m_buffer; }

		constiterator end() const { return alloc.m_buffer + m_count; }
		iterator end() { return alloc.m_buffer +m_count; }

		reviterator rbegin() { return alloc.m_buffer + m_count; }
		reviterator rend() { return alloc.m_buffer; }

		constreviterator rbegin() const { return alloc.m_buffer + m_count; }
		constreviterator rend() const { return alloc.m_buffer; }

		//Is the array empty?
		[[nodiscard("This function does not empty the array.")]] bool empty() const { return begin() == end(); }

		//Reallocate enough memory for the provided number of elements.
		void reserve(size_type cap)
		{
			T* temp = alloc.m_buffer;

			alloc.allocate(cap);

			for (size_type i = 0; i < m_count; i++)
			{
				new(&alloc.m_buffer[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			alloc.deallocate(temp, m_capacity);

			m_capacity = cap;
		}

		//Store a copy of the provided object in the array.
		void push_back(const T& element)
		{
			EmplaceBack(element);
		}

		//Move the provided object into the array.
		void push_back(T&& element)
		{
			emplace_back(std::move(element));
		}

		//Construct an object of type T directly into the array using the
		//provided arguments.
		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_count >=m_capacity)
				grow();

			new(&alloc.m_buffer[m_count]) T(std::forward<Args>(args)...);

			++m_count;
		}

		//Erase the last element. Similar to pop_back.
		void pop_back()
		{
			if (m_count > 0) --m_count;
			
			alloc.m_buffer[m_count].~T();
		}

		//Erase all elements from the starting point to the stopping point.
		//If start is nullptr, this will start at the begining of the array.
		//If stop is nullptr, this will stop at the end of the array.
		void erase(T* start, T* stop = nullptr)
		{
			if(stop == nullptr) stop = end();
			if (start == end()) return;
			if (start == nullptr) start = begin();

			while (start != stop)
			{
				start->~T();
				m_count--;
				start++;
			}
		}

		//clears the whole array and resets the pushm_counter.
		void clear()
		{
			for (size_type i = 0; i < m_count; i++)
				alloc.m_buffer[i].~T();

			m_count = 0;
		}
	};

	//Used for move operations.
	template<typename T>
	void nullify_array(const dynarr<T>& arr)
	{
		arr.count = 0;
		arr.capacity = 0;
		arr.alloc.m_buffer = nullptr;
	}

	template<os_compatible T, size_t N>
	inline std::ostream& operator<<(std::ostream& stream, const array<T, N>& obj)
	{
		stream << "{ ";

		for(size_t ind = 0; ind < N - 1; ++ind)
			stream << obj[ind] << ", ";
		
		stream << obj[N-1] << " }";

		return stream;
	}

	template<os_compatible T>
	inline std::ostream& operator<<(std::ostream& stream, const dynarr<T>& obj)
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
