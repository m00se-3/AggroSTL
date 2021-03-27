#include <utility>
#include <initializer_list>
#include <optional>
#include "concepts/StreamConcepts.hpp"

namespace aggro
{
	enum class ArrayExpandMethod : uint8_t
	{
		INCREMENT = 0, PLUS_HALF, DOUBLE
	};

	//Stack allocated array which supports iterators. Replaces std::array.
	template<typename T, unsigned int N>
	class StArray
	{
		using TypeValue = T;
		using Iterator = T*;
		using ConstIterator = const T*;
		using RevIterator = T*;
		using ConstRevIterator = const T*;

		T data[N];

	public:
		StArray() = default;
		StArray(const StArray& other)
		{
			for (int n = 0; n < N; n++)
				data[n] = other[n];
		}

		StArray(StArray&& other) noexcept
		{
			for (int n = 0; n < N; n++)
				data[n] = other[n];
		}

		StArray(const std::initializer_list<T>& inits)
		{
			for (int n = 0; n < N; n++)
				new(&data[n]) T(std::move(*(inits.begin() + n)));
		}
		~StArray() = default;

		StArray& operator=(const StArray& other)
		{
			for (int n = 0; n < N; n++)
				data[n] = other[n];

			return *this;
		}

		StArray& operator=(StArray&& other) noexcept
		{
			for (int n = 0; n < N; n++)
				data[n] = other[n];

			return *this;
		}

		StArray& operator=(const std::initializer_list<T>& inits)
		{
			for (int n = 0; n < N; n++)
				new(&data[n]) T(std::move(*(inits.begin() + n)));

			return *this;
		}

		T& operator[](size_t index) { return data[index]; }
		const T& operator[](size_t index) const { return data[index]; }

		constexpr size_t Size() const { return N; }
		constexpr size_t Bytes() const { return N * sizeof(T); }

		T* Data() { return data; }

		std::optional<T> At(size_t index)
		{
			if (index < N)
				return data[index];
			else
				return std::nullopt;
		}

		[[nodiscard("This function does not empty the array.")]] bool Empty() const { return begin() == end(); }

		Iterator begin() { return data; }
		Iterator end() { return data + N; }

		ConstIterator begin() const { return data; }
		ConstIterator end() const { return data + N; }

		RevIterator rbegin() { return data + N; }
		RevIterator rend() { return data; }

		ConstRevIterator rbegin() const { return data + N; }
		ConstRevIterator rend() const { return data; }

		
	};

	/*
		Dynamically allocated array used to replace std::vector. This class does not support
		custom allocators. By default the DyArrays grow exponentially, reducing calls to new.
		The method used to resize the array upon adding a new element can be changed using the ArrayType enum.
		This class allocates 3 Ts worth of memory on creation by default.
	*/
	template<typename T>
	class DyArray
	{
		using Iterator = T*;
		using ConstIterator = const T*;
		using RevIterator = T*;
		using ConstRevIterator = const T*;

		T* data = nullptr;
		size_t count = 0;
		size_t capacity = 3;

		void Grow()
		{
			T* temp = data;
			size_t nCap;
			size_t oCap = capacity;

			auto decide = [](size_t test) -> size_t {
				if (test > 0)
					return test;
				else
					return 1;
			};

			switch (ExpandMethod)
			{
			case ArrayExpandMethod::INCREMENT:
				nCap = decide(capacity + 1);
				break;
			case ArrayExpandMethod::PLUS_HALF:
				nCap = decide(capacity + (capacity / 2));
				break;
			case ArrayExpandMethod::DOUBLE:
				nCap = decide(capacity * 2);
				break;
			}

			data = Allocate(nCap);
			capacity = nCap;

			for (int i = 0; i < count; i++)
			{
				new(&data[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			::operator delete(temp, oCap * sizeof(T));
		}

		T* Allocate(size_t cap)
		{
			return static_cast<T*>(::operator new(cap * sizeof(T)));
		}

	public:

		friend void NullifyArray(const DyArray& arr);

		ArrayExpandMethod ExpandMethod = ArrayExpandMethod::PLUS_HALF;

		DyArray()
		{
			data = Allocate(capacity);
		}

		DyArray(size_t cap)
			: capacity(cap)
		{
			data = Allocate(cap);
		}

		DyArray(std::initializer_list<T>&& inits)
			: capacity(inits.size()), count(0)
		{
			data = Allocate(capacity);
			for (int i = 0; i < capacity; i++)
			{
				new(&data[i]) T(std::move(*(inits.begin() + i)));
				++count;
			}
		}

		DyArray(const DyArray& other)
			: count(other.Size()), capacity(other.Capacity())
		{
			data = Allocate(capacity);

			if (data != nullptr)
			{
				for (int i = 0; i < count; i++)
				{
					new(&data[i]) T(other.Data()[i]);
				}
			}
		}

		DyArray(DyArray&& other) noexcept
			: count(other.Size()), capacity(other.Capacity())
		{
			data = other.Data();
			NullifyArray(other);
		}

		T& operator[](size_t index)
		{
			return data[index];
		}

		const T& operator[](size_t index) const
		{
			return data[index];
		}

		DyArray& operator=(const DyArray& other)
		{
			if (this != &other)
			{
				Clear();
				::operator delete(data, capacity * sizeof(T));


				count = other.Size();
				capacity = other.Capacity();

				data = Allocate(capacity);

				if (data != nullptr)
				{
					for (int i = 0; i < count; i++)
					{
						new(&data[i]) T(other.Data()[i]);
					}
				}
			}

			return *this;
		}

		DyArray& operator=(DyArray&& other) noexcept
		{
			if (this != &other)
			{
				Clear();
				::operator delete(data, capacity * sizeof(T));

				count = other.Size();
				capacity = other.Capacity();

				data = other.Data();

				NullifyArray(other);
			}

			return *this;
		}

		DyArray& operator=(std::initializer_list<T>&& list)
		{
			Clear();
			::operator delete(data, capacity * sizeof(T));

			capacity = list.Size();
			data = Allocate(capacity);

			for (int i = 0; i < capacity; i++)
			{
				new(&data[i]) T(std::move(*(list.begin() + i)));
				++count;
			}

			return *this;
		}

		~DyArray()
		{
			Clear();
			::operator delete(data, capacity * sizeof(T));
			capacity = 0;
		}

		std::optional<T> At(size_t index)
		{
			if (index < count)
				return data[index];
			else
				return std::nullopt;
		}

		//Return the first element in the array.
		T& First() const { return data[0]; }

		//Return the last element in the array.
		T& Last() const { return data[count - 1]; }

		//Number of elements contained.
		size_t Size() const
		{
			return count;
		}

		//Number of elements able to be held.
		size_t Capacity() const
		{
			return capacity;
		}

		//Total size in bytes
		size_t Bytes() const
		{
			return count * sizeof(T);
		}

		//Return raw pointer to the array data.
		const T* Data() const { return data; }
		T* Data() { return data; }

		ConstIterator begin() const { return data; }
		Iterator begin() { return data; }

		ConstIterator end() const { return data + count; }
		Iterator end() { return data + count; }

		RevIterator rbegin() { return data + count; }
		RevIterator rend() { return data; }

		ConstRevIterator rbegin() const { return data + count; }
		ConstRevIterator rend() const { return data; }

		//Is the array empty?
		[[nodiscard("This function does not empty the array.")]] bool Empty() const { return begin() == end(); }

		//Allocate enough memory for the provided number of elements.
		void Reserve(size_t cap)
		{
			T* temp = data;

			data = Allocate(cap);

			for (int i = 0; i < count; i++)
			{
				new(&data[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			::operator delete(temp, capacity * sizeof(T));

			capacity = cap;
		}

		//Store a copy of the provided object in the array.
		//Similar to push_back.
		void PushBack(const T& element)
		{
			EmplaceBack(element);
		}

		//Move the provided object into the array. Similar to push_back.
		void PushBack(T&& element)
		{
			EmplaceBack(std::move(element));
		}

		//Construct an object of type T directly into the array using the
		//provided arguments. Similar to emplace_back.
		template<typename... Args>
		void EmplaceBack(Args&&... args)
		{
			if (count >= capacity)
				Grow();

			new(&data[count]) T(std::forward<Args>(args)...);

			++count;
		}

		//Erase the last element. Similar to pop_back.
		void PopBack()
		{
			if (count > 0) --count;
			
			data[count].~T();
		}

		//Erase all elements from the starting point to the stopping point.
		//If start is nullptr, this will start at the begining of the array.
		//If stop is nullptr, this will stop at the end of the array.
		void Erase(T* start, T* stop = end())
		{
			if (start == end()) return;
			if (start == nullptr) start = begin();

			while (start != stop)
			{
				start->~T();
				count--;
				start++;
			}
		}

		//Clears the whole array and resets the push counter.
		void Clear()
		{
			for (int i = 0; i < count; i++)
				data[i].~T();

			count = 0;
		}
	};

	//Used for move operations.
	template<typename T>
	void NullifyArray(const DyArray<T>& arr)
	{
		arr.count = 0;
		arr.capacity = 0;
		arr.data = nullptr;
	}

	template<OStreamCompatible T, size_t N>
	inline std::ostream& operator<<(std::ostream& stream, const StArray<T, N>& obj)
	{
		stream << "{ ";

		for(int ind = 0; ind < N - 1; ++ind)
			stream << obj[ind] << ", ";
		
		stream << obj[N-1] << " }";

		return stream;
	}

	template<OStreamCompatible T>
	inline std::ostream& operator<<(std::ostream& stream, const DyArray<T>& obj)
	{
		stream << "{ ";

		if(obj.Size() > 0)
		{
			size_t size = obj.Size() - 1;

			for(int ind = 0; ind < size; ++ind)
				stream << obj[ind] << ", ";
			
			stream << obj[size];
		}
			stream  << " }";

		return stream;
	}
}
