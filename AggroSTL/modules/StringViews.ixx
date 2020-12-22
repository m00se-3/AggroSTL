module;

#include <type_traits>
#include <memory>
#include <optional>

export module StringViews;

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
		The stringView class is similar to the std::string_view class.
	*/
	export template<Character T>
		class stringView
	{
		const T* m_ptr = nullptr;
		size_t m_size = 0;

	public:

		using constIterator = const T*;
		using constRevIterator = const T*;

		stringView() = default;
		stringView(const T* inStr, size_t inSize)
			: m_ptr(inStr), m_size(inSize)
		{
		}

		const T& operator[](size_t index) const { return *(m_ptr + index); }


		// Returns character at index with bounds checking.
		const std::optional<T>& at(size_t index)
		{
			if (index < m_size)
				return *(m_ptr + index);
			else
				return {};
		}

		// Return the view pointer.
		const T* data() const { return m_ptr; }

		constIterator begin() const { return m_ptr; }
		constIterator end() const { return m_ptr + m_size; }

		constRevIterator rbegin() const { return m_ptr + m_size; }
		constRevIterator rend() const { return m_ptr; }
	};
}