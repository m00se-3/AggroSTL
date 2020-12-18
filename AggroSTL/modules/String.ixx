module;

#include <type_traits>
#include <memory>
#include <optional>

export module String;

/*
	TODO: implement helpful functions to the string classes, examples:
		size_t find() : return the first occurance of a substring in a string.
		size_t next() : coroutine returning occurances of substrings.
		operator+() : concatinate strings together.

	TODO: create a std::string_view like class.
*/

namespace aggro
{

	template<typename T>
	concept Character = requires
	{
		std::is_same<T, char>::value ||
			std::is_same<T, char8_t>::value ||
			std::is_same<T, char16_t>::value ||
			std::is_same<T, char32_t>::value;
	};

	/*
		A basic string type similar to std::string. This class is designed to support
		all character encoding from ASCII to Unicode-32.
	*/
	export template<Character T> 
		class stringType
	{
		T* m_data = nullptr;

		size_t m_size = 0;
		size_t m_capacity = 0;

	public:

		using iterator = T*;
		using constIterator = const T*;
		using revIterator = T*;
		using constRevIterator = const T*;

		stringType() = default;
		stringType(T* input)
			: m_size(strlen(input))
		{
			m_capacity = m_size;
			m_data = new T[m_size + 1];

			strcpy(m_data, input);
		}

		stringType(const stringType& other)
			: m_size(other.length()), m_capacity(other.capacity())
		{
			m_data = new T[m_capacity + 1];

			strncpy(m_data, other.data(), m_capacity);
		}

		stringType(stringType&& other) noexcept
			: m_data(other.data()), m_size(other.length()), m_capacity(other.capacity())
		{
			other.nullify();
		}

		stringType(T* input, size_t cap)
			: m_capacity(cap), m_size(strlen(input))
		{
			m_data = new T[m_capacity + 1];

			strncpy(m_data, input, m_capacity);
		}

		~stringType()
		{
			delete[] m_data;
			m_size = 0;
			m_capacity = 0;
		}

		stringType& operator=(T* input)
		{
			m_size = strlen(input);

			if (m_capacity < m_size)
			{
				delete[] m_data;
				m_capacity = m_size;
				m_data = new T[m_capacity + 1];
			}

			m_data = new T[m_size + 1];

			strncpy(m_data, input, m_capacity);

			return *this;
		}

		stringType& operator=(const stringType& other)
		{
			if (m_capacity < other.capacity())
			{
				delete[] m_data;
				m_capacity = other.capacity();
				m_data = new T[m_capacity + 1];
			}

			m_size = other.length();

			strncpy(m_data, other.data(), m_capacity);

			return *this;
		}

		stringType& operator=(stringType&& other) noexcept
		{
			if (m_data)
				delete[] m_data;

			m_size = other.length();
			m_capacity = other.capacity();
			m_data = other.data();
			other.nullify();

			return *this;
		}

		T& operator[](size_t index) { return m_data[index]; }

		const T& operator[](size_t index) const { return m_data[index]; }

		// Return the specified character with bounds checking.
		std::optional<T&> at(size_t index)
		{
			if (index <= m_size)
				return m_data[index];
			else
				return {};
		}

		// Return the specified character with bounds checking.
		const std::optional<T&> at(size_t index) const
		{
			if (index <= m_size)
				return m_data[index];
			else
				return {};
		}

		// Return the number of characters in the string.
		size_t length() const { return m_size; }

		//Returns the number of characters the current object can hold.
		size_t capacity() const { return m_capacity; }

		// Return the size of the string in bytes.
		size_t bytes() const { return m_size * sizeof(T); }

		// Returns the number of bytes occupied by the entire object, including unsused memory.
		size_t totalBytes() const { return (m_capacity + 1) * sizeof(T); }

		// Checks if the passed substring exists in the string object.
		bool contains(T* sub) const
		{
			size_t sublength = strlen(sub);
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sublength) break;
			}

			return count == sublength;
		}

		// Checks if the passed substring exists in the string object.
		bool contains(const stringType& sub) const
		{
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sub.length()) break;
			}

			return count == sub.length();
		}

		// Checks if the passed substring exists in the string object.
		bool contains(const sharedStringType<T>& sub) const
		{
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sub.length()) break;
			}

			return count == sub.length();
		}

		// Return the raw character pointer.
		T* data() { return m_data; }

		// Return the raw character pointer.
		const T* data() const { return m_data; }

		iterator begin() { return m_data; }
		iterator end() { return m_data + m_size; }

		constIterator begin() const { return m_data; }
		constIterator end() const { return m_data + m_size; }

		revIterator rbegin() { return m_data + m_size; }
		revIterator rend() { return m_data; }

		constRevIterator rbegin() const { return m_data + m_size; }
		constRevIterator rend() const { return m_data; }

		// Removes the internal pointer to the string data.
		void nullify() 
		{
			m_data = nullptr; 
			m_size = 0;
			m_capacity = 0;
		}
	};



	/*
		A string class similar to aggro::string, but is capable of sharing ownership of
		it's internal data with other sharedString objects.
	*/
	export template<Character T>
		class sharedStringType
	{
		std::shared_ptr<T> m_data = nullptr;

		size_t m_size = 0;
		size_t m_capacity = 0;

	public:

		using iterator = T*;
		using constIterator = const T*;
		using revIterator = T*;
		using constRevIterator = const T*;

		sharedStringType() = default;
		sharedStringType(T* input)
			: m_size(strlen(input))
		{
			m_capacity = m_size;
			m_data.reset(new T[m_size + 1]);

			strcpy(m_data.get(), input);
		}

		sharedStringType(const sharedStringType& other)
		{
			m_data = other.data();
			m_size = other.length();
			m_capacity = other.capacity();
		}

		sharedStringType(sharedStringType&& other) noexcept
		{
			m_data = std::move(other.data());
			m_size = other.length();
			m_capacity = other.capacity();
		}

		~sharedStringType()
		{
			// Smart pointers are beautiful.
		}

		sharedStringType& operator=(T* input)
		{
			m_size = strlen(input);
			
			if (m_capacity < m_size)
			{
				m_capacity = m_size;
				m_data.reset(new T[m_size + 1]);
			}

			strncpy(m_data.get(), input, m_capacity);

			return *this;
		}

		sharedStringType& operator=(const sharedStringType& other)
		{ 
			m_data = other.data(); 
			m_size = other.length();
			m_capacity = other.capacity();
			return *this;
		}

		sharedStringType& operator=(sharedStringType&& other) noexcept
		{ 
			m_data = std::move(other.data());
			m_size = other.length();
			m_capacity = other.capacity();
			other.nullify();

			return *this;
		}

		// Returns the length of the current string.
		size_t length() const { return m_size; }

		// Return the number of characters the current object can hold.
		size_t capacity() const { return m_capacity; }

		// Returns the number of bytes the current string occupies.
		size_t bytes() const { return m_size * sizeof(T); }

		// Returns the number of bytes occupied by the entire object, including unsused memory.
		size_t totalBytes() const { return (m_capacity + 1) * sizeof(T); }

		// Checks if the passed substring exists in the string object.
		bool contains(T* sub) const
		{
			size_t sublength = strlen(sub);
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sublength) break;
			}

			return count == sublength;
		}

		// Checks if the passed substring exists in the string object.
		bool contains(const stringType& sub) const
		{
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sub.length()) break;
			}

			return count == sub.length();
		}

		// Checks if the passed substring exists in the string object.
		bool contains(const sharedStringType<T>& sub) const
		{
			size_t count = 0;

			for (auto c : m_data)
			{
				if (c == sub[count])
					count++;
				else
					count = 0;

				if (count == sub.length()) break;
			}

			return count == sub.length();
		}

		// Returns the internal std::shared_ptr to the string.
		std::shared_ptr<T> data() { return m_data; }

		// Returns the internal std::shared_ptr to the string.
		const std::shared_ptr<T> data() const { return m_data; }

		iterator begin() { return m_data; }
		iterator end() { return m_data + m_size; }

		constIterator begin() const { return m_data; }
		constIterator end() const { return m_data + m_size; }

		revIterator rbegin() { return m_data + m_size; }
		revIterator rend() { return m_data; }

		constRevIterator rbegin() const { return m_data + m_size; }
		constRevIterator rend() const { return m_data; }

		// Returns a copy of the current string seperate from the owned object.
		stringType<T> copy() const
		{
			return stringType(m_data.get(), m_capacity);
		}

		// Removes onwership of the string object.
		void nullify()
		{
			m_size = 0;
			m_capacity = 0;
			m_data.reset();
		}
	};

	// Type aliases for the stringType and sharedStringType class templates.

	export using string = stringType<char>;
	export using stringU8 = stringType<char8_t>;
	export using stringU16 = stringType<char16_t>;
	export using stringU32 = stringType<char32_t>;

	export using sharedString = sharedStringType<char>;
	export using sharedStringU8 = sharedStringType<char8_t>;
	export using sharedStringU16 = sharedStringType<char16_t>;
	export using sharedStringU32 = sharedStringType<char32_t>;
}