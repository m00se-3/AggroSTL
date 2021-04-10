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
	class StArray
	{
	
	private:
		using TypeValue = T;
		using Iterator = T*;
		using ConstIterator = const T*;
		using RevIterator = T*;
		using ConstRevIterator = const T*;

		T data[N];

	public:
		using SizeType = std::size_t;

		StArray() = default;
		StArray(const StArray& other)
		{
			for (SizeType n = 0; n < Size(); n++)
				data[n] = other[n];
		}

		StArray(StArray&& other) noexcept
		{
			for (SizeType n = 0; n < Size(); n++)
				data[n] = other[n];
		}

		StArray(std::initializer_list<T>&& inits)
		{
			for (SizeType n = 0; n < Size(); n++)
				new(&data[n]) T(std::move(*(inits.begin() + n)));
		}
		~StArray() = default;

		StArray& operator=(const StArray& other)
		{
			for (SizeType n = 0; n < Size(); n++)
				data[n] = other[n];

			return *this;
		}

		StArray& operator=(StArray&& other) noexcept
		{
			for (SizeType n = 0; n < Size(); n++)
				data[n] = other[n];

			return *this;
		}

		StArray& operator=(std::initializer_list<T>&& inits)
		{
			for (SizeType n = 0; n < Size(); n++)
				new(&data[n]) T(std::move(*(inits.begin() + n)));

			return *this;
		}

		T& operator[](SizeType index) 
		{
			return data[index]; 
		}

		const T& operator[](SizeType index) const 
		{
			return data[index]; 
		}

		constexpr SizeType Size() const { return N; }
		constexpr SizeType Bytes() const { return N * sizeof(T); }

		T* Data() { return data; }

		std::optional<T> At(SizeType index)
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
		Dynamically allocated array which replaces std::vector. By default the DyArrays grow exponentially, reducing calls to new.
		The method used to resize the array upon adding a new element can be changed using the ArrayExpandMethod enum.
	*/
	template<typename T, StandardAllocator Alloc = STDAllocator<T>>
	class DyArray
	{
	public:
		using SizeType = std::size_t;

	private:

		using Iterator = T*;
		using ConstIterator = const T*;
		using RevIterator = T*;
		using ConstRevIterator = const T*;
		using Resource = Alloc;

		Resource alloc;
		SizeType count {};
		SizeType capacity {};

		void Grow()
		{
			T* temp = alloc.Buffer;
			SizeType nCap;
			SizeType oCap = capacity;

			auto decide = [](SizeType test) -> SizeType {
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

			alloc.Allocate(nCap);
			capacity = nCap;

			for (SizeType i = 0; i < count; i++)
			{
				new(&alloc.Buffer[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			alloc.Deallocate(temp, oCap);
		}

	public:

		friend void NullifyArray(const DyArray& arr);

		ArrayExpandMethod ExpandMethod = ArrayExpandMethod::PLUS_HALF;

		DyArray() = default;

		DyArray(SizeType cap)
			: capacity(cap)
		{
			alloc.Allocate(cap);
		}

		DyArray(std::initializer_list<T>&& inits)
			: count(0), capacity(inits.size())
		{
			alloc.Allocate(capacity);
			for (SizeType i = 0; i < capacity; i++)
			{
				new(&alloc.Buffer[i]) T(std::move(*(inits.begin() + i)));
				++count;
			}
		}

		DyArray(const DyArray& other)
			: count(other.Size()), capacity(other.Capacity())
		{
			alloc.Allocate(capacity);

			if (alloc.Buffer != nullptr)
			{
				for (SizeType i = 0; i < count; i++)
				{
					new(&alloc.Buffer[i]) T(other.Data()[i]);
				}
			}
		}

		DyArray(DyArray&& other) noexcept
			: count(other.Size()), capacity(other.Capacity())
		{
			alloc.Buffer = other.Data();
			NullifyArray(other);
		}

		T& operator[](SizeType index)
		{
			return alloc.Buffer[index];
		}

		const T& operator[](SizeType index) const
		{
			return alloc.Buffer[index];
		}

		DyArray& operator=(const DyArray& other)
		{
			if (this != &other)
			{
				Clear();
				alloc.Deallocate(capacity);


				count = other.Size();
				capacity = other.Capacity();

				alloc.Allocate(capacity);

				if (alloc.Buffer != nullptr)
				{
					for (SizeType i = 0; i < count; i++)
					{
						new(&alloc.Buffer[i]) T(other.Data()[i]);
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
				alloc.Deallocate(capacity);

				count = other.Size();
				capacity = other.Capacity();

				alloc.Buffer = other.Data();

				NullifyArray(other);
			}

			return *this;
		}

		DyArray& operator=(std::initializer_list<T>&& list)
		{
			Clear();
			
			if(list.size() > Capacity())
			{
				alloc.Deallocate(capacity);
				capacity = list.size();
				alloc.Allocate(capacity);
			}

			for (SizeType i = 0; i < capacity; i++)
			{
				new(&alloc.Buffer[i]) T(std::move(*(list.begin() + i)));
				++count;
			}

			return *this;
		}

		~DyArray()
		{
			Clear();
			alloc.Deallocate(capacity);
			capacity = 0;
		}

		std::optional<T> At(SizeType index)
		{
			if (index < count)
				return alloc.Buffer[index];
			else
				return std::nullopt;
		}

		//Return the first element in the array.
		T& First() const { return alloc.Buffer[0]; }

		//Return the last element in the array.
		T& Last() const { return alloc.Buffer[count - 1]; }

		//Number of elements contained.
		SizeType Size() const
		{
			return count;
		}

		//Number of elements able to be held.
		SizeType Capacity() const
		{
			return capacity;
		}

		//Total size in bytes
		size_t Bytes() const
		{
			return count * sizeof(T);
		}

		//Return raw pointer to the array data.
		const T* Data() const { return alloc.Buffer; }
		T* Data() { return alloc.Buffer; }

		ConstIterator begin() const { return alloc.Buffer; }
		Iterator begin() { return alloc.Buffer; }

		ConstIterator end() const { return alloc.Buffer + count; }
		Iterator end() { return alloc.Buffer + count; }

		RevIterator rbegin() { return alloc.Buffer + count; }
		RevIterator rend() { return alloc.Buffer; }

		ConstRevIterator rbegin() const { return alloc.Buffer + count; }
		ConstRevIterator rend() const { return alloc.Buffer; }

		//Is the array empty?
		[[nodiscard("This function does not empty the array.")]] bool Empty() const { return begin() == end(); }

		//Reallocate enough memory for the provided number of elements.
		void Reserve(SizeType cap)
		{
			T* temp = alloc.Buffer;

			alloc.Allocate(cap);

			for (SizeType i = 0; i < count; i++)
			{
				new(&alloc.Buffer[i]) T(std::move(temp[i]));
				temp[i].~T();
			}

			alloc.Deallocate(temp, capacity);

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

			new(&alloc.Buffer[count]) T(std::forward<Args>(args)...);

			++count;
		}

		//Erase the last element. Similar to pop_back.
		void PopBack()
		{
			if (count > 0) --count;
			
			alloc.Buffer[count].~T();
		}

		//Erase all elements from the starting point to the stopping point.
		//If start is nullptr, this will start at the begining of the array.
		//If stop is nullptr, this will stop at the end of the array.
		void Erase(T* start, T* stop = nullptr)
		{
			if(stop == nullptr) stop = end();
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
			for (SizeType i = 0; i < count; i++)
				alloc.Buffer[i].~T();

			count = 0;
		}
	};

	//Used for move operations.
	template<typename T>
	void NullifyArray(const DyArray<T>& arr)
	{
		arr.count = 0;
		arr.capacity = 0;
		arr.alloc.Buffer = nullptr;
	}

	template<OStreamCompatible T, size_t N>
	inline std::ostream& operator<<(std::ostream& stream, const StArray<T, N>& obj)
	{
		stream << "{ ";

		for(size_t ind = 0; ind < N - 1; ++ind)
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
			auto size = obj.Size() - 1;

			for(size_t ind = 0; ind < size; ++ind)
				stream << obj[ind] << ", ";
			
			stream << obj[size];
		}
			stream  << " }";

		return stream;
	}
}
