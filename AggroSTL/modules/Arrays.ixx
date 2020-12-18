module;

#include <utility>
#include <initializer_list>
#include <optional>

export module Arrays;

namespace aggro
{
	export enum class arrayType
	{
		TINY, SMALL, LARGE
	};

	//Stack allocated array which supports iterators. Replaces std::array.
	export template<typename T, unsigned int N>
	class stArray
	{
		using typeValue = T;
		using iterator = T*;
		using constIterator = const T*;
		using revIterator = T*;
		using constRevIterator = const T*;

		T m_data[N];

	public:
		stArray() = default;
		stArray(const stArray& other)
		{
			for (int n = 0; n < N; n++)
				m_data[n] = other[n];
		}

		stArray(stArray&& other) noexcept
		{
			for (int n = 0; n < N; n++)
				m_data[n] = other[n];
		}

		stArray(const std::initializer_list<T>& inits)
		{
			for (int n = 0; n < N; n++)
				m_data[n] = *(inits.begin() + n);
		}
		~stArray() {}

		stArray& operator=(const stArray& other)
		{
			for (int n = 0; n < N; n++)
				m_data[n] = other[n];

			return *this;
		}

		stArray& operator=(stArray&& other) noexcept
		{
			for (int n = 0; n < N; n++)
				m_data[n] = other[n];

			return *this;
		}

		stArray& operator=(const std::initializer_list<T>& inits)
		{
			for (int n = 0; n < N; n++)
				m_data[n] = *(inits.begin() + n);

			return *this;
		}

		T& operator[](size_t index) { return m_data[index]; }
		const T& operator[](size_t index) const { return m_data[index]; }

		constexpr size_t size() const { return N; }
		constexpr size_t bytes() const { return N * sizeof(T); }

		T* data() { return m_data; }
		std::optional<T> at(size_t index)
		{
			if (index < N)
				return m_data[index];
			else
				return {};
		}

		bool empty() const { return begin() == end(); }

		iterator begin() { return m_data; }
		iterator end() { return m_data + N; }

		constIterator begin() const { return m_data; }
		constIterator end() const { return m_data + N; }

		revIterator rbegin() { return m_data + N; }
		revIterator rend() { return m_data; }

		constRevIterator rbegin() const { return m_data + N; }
		constRevIterator rend() const { return m_data; }
	};

	/*
		Dynamically allocated array used to replace std::vector. This class does not support
		custom allocators, but allows you to configure how liberal the array is with memory.
		Larger array types allocate more memory per resize but resize less often as a result.
		This class allocates 3 Ts worth of memory on creation by default.
	*/
	export template<typename T, arrayType A = arrayType::SMALL>
	class dyArray
	{
		using iterator = T*;
		using constIterator = const T*;
		using revIterator = T*;
		using constRevIterator = const T*;

		T* m_data = nullptr;
		size_t m_count = 0;
		size_t m_capacity = 3;

		void grow()
		{
			T* temp = m_data;
			size_t nCap;
			size_t oCap = m_capacity;

			auto decide = [](size_t test) {
				if (test > 0)
					return test;
				else
					return (size_t)1;
			};

			switch (A)
			{
			case arrayType::TINY:
				nCap = decide(m_capacity + 1);
				break;
			case arrayType::SMALL:
				nCap = decide(m_capacity + (m_capacity / 2));
				break;
			case arrayType::LARGE:
				nCap = decide(m_capacity * 2);
				break;
			}

			m_data = allocate(nCap);
			m_capacity = nCap;

			for (int i = 0; i < m_count; i++)
			{
				new(&m_data[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			::operator delete(temp, oCap * sizeof(T));
		}

		T* allocate(size_t cap)
		{
			return (T*)::operator new(cap * sizeof(T));
		}

	public:
		dyArray()
		{
			m_data = allocate(m_capacity);
		}

		dyArray(size_t cap)
			: m_capacity(cap)
		{
			m_data = allocate(cap);
		}

		dyArray(std::initializer_list<T>&& inits)
			: m_capacity(inits.size())
		{
			m_data = allocate(m_capacity);
			for (int i = 0; i < m_capacity; i++)
			{
				m_data[i] = *(inits.begin() + i);
				m_count++;
			}
		}

		dyArray(const dyArray& other)
			: m_count(other.size()), m_capacity(other.capacity())
		{
			m_data = allocate(m_capacity);

			if (m_data != nullptr)
			{
				for (int i = 0; i < m_count; i++)
				{
					new(&m_data[i]) T(other.data()[i]);
				}
			}
		}

		dyArray(dyArray&& other)
			: m_count(other.size()), m_capacity(other.capacity())
		{
			m_data = other.data();
			other.nullify();
		}

		T& operator[](size_t index)
		{
			return m_data[index];
		}

		const T& operator[](size_t index) const
		{
			return m_data[index];
		}

		dyArray& operator=(const dyArray& other)
		{
			if (this != &other)
			{
				clear();
				::operator delete(m_data, m_capacity * sizeof(T));


				m_count = other.size();
				m_capacity = other.capacity();

				m_data = allocate(m_capacity);

				if (m_data != nullptr)
				{
					for (int i = 0; i < m_count; i++)
					{
						new(&m_data[i]) T(other.data()[i]);
					}
				}
			}

			return *this;
		}

		dyArray& operator=(dyArray&& other) noexcept
		{
			if (this != &other)
			{
				clear();
				::operator delete(m_data, m_capacity * sizeof(T));

				m_count = other.size();
				m_capacity = other.sapacity();

				m_data = other.data();

				other.nullify();
			}

			return *this;
		}

		dyArray& operator=(std::initializer_list<T>&& list)
		{
			clear();
			::operator delete(m_data, m_capacity * sizeof(T));

			m_capacity = list.size();
			m_data = allocate(m_capacity);

			for (int i = 0; i < m_capacity; i++)
			{
				m_data[i] = *(list.begin() + i);
				m_count++;
			}

			return *this;
		}

		~dyArray()
		{
			clear();
			::operator delete(m_data, m_capacity * sizeof(T));
			m_capacity = 0;
		}

		//Return the first element in the array.
		T& first() const { return m_data[0]; }

		//Return the last element in the array.
		T& last() const { return m_data[m_count - 1]; }

		//Number of elements contained.
		size_t size() const
		{
			return m_count;
		}

		//Number of elements able to be held.
		size_t capacity() const
		{
			return m_capacity;
		}

		//Total size in bytes
		size_t bytes() const
		{
			return m_count * sizeof(T);
		}

		//Return raw pointer to the array data.
		const T* data() const { return m_data; }
		T* data() { return m_data; }

		constIterator begin() const { return m_data; }
		iterator begin() { return m_data; }

		constIterator end() const { return m_data + m_count; }
		iterator end() { return m_data + m_count; }

		revIterator rbegin() { return m_data + m_count; }
		revIterator rend() { return m_data; }

		constRevIterator rbegin() const { return m_data + m_count; }
		constRevIterator rend() const { return m_data; }

		//Is the array empty?
		bool empty() const { return begin() == end(); }

		//Allocate enough memory for the provided number of elements.
		void reserve(size_t cap)
		{
			T* temp = m_data;

			m_data = allocate(cap);

			for (int i = 0; i < m_count; i++)
			{
				new(&m_data[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			::operator delete(temp, m_capacity * sizeof(T));

			m_capacity = cap;
		}

		//Store a copy of the provided object in the array.
		//Similar to push_back.
		void pushBack(const T& element)
		{
			emplaceBack(element);
		}

		//Move the provided object into the array. Similar to push_back.
		void pushBack(T&& element)
		{
			emplaceBack(std::move(element));
		}

		//Construct an object of type T directly into the array using the
		//provided arguments. Similar to emplace_back.
		template<typename... Args>
		void emplaceBack(Args&&... args)
		{
			if (m_count >= m_capacity)
				grow();

			new(&m_data[m_count]) T(std::forward<Args>(args)...);

			m_count++;
		}

		//Erase the last element. Similar to pop_back.
		void popBack()
		{
			if (m_count > 0)
			{
				m_count--;
				m_data[m_count].~T();
			}
			else
			{
				m_data[m_count].~T();
			}
		}

		//Erase all elements from the starting point to the stopping point.
		//If start is nullptr, this will start at the begining of the array.
		//If stop is nullptr, this will stop at the last pushed element.
		void erase(T* start, T* stop = nullptr)
		{
			if (start == end()) return;
			if (start == nullptr) start = begin();
			if (stop == nullptr) stop = end();

			while (start < stop)
			{
				start->~T();
				m_count--;
				start++;
			}
		}

		//Clears the whole array and resets the push counter.
		void clear()
		{
			for (int i = 0; i < m_count; i++)
				m_data[i].~T();

			m_count = 0;
		}

		//Used for move operations. Using it in your code is not recommended.
		void nullify()
		{
			m_count = 0;
			m_capacity = 0;
			m_data = nullptr;
		}
	};
}
