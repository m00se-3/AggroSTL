#ifndef STREAMCONCEPTS_HPP
#define STREAMCONCEPTS_HPP

#include <concepts>
#include <ostream>

namespace aggro
{
    /*
		This concept is satisfied if an overload exists for the ostream(<<) operator.
		Plain Old Datatypes(PODs) also satisfy this constraint.
	*/
	template<typename T> concept os_compatible = requires (std::ostream os, T type)
	{
		{ os << type } -> std::same_as<std::ostream&>;
	};

    /*
		This concept is satisfied if an overload exists for the istream(>>) operator.
		Plain Old Datatypes(PODs) also satisfy this constraint.
	*/
	template<typename T> concept is_compatible = requires (std::istream is, T type)
	{
		{ is << type } -> std::same_as<std::istream&>;
	};

	/*
		This concept is satisfied if both stream operators (<< and >>) are implemented.
		Plain Old Datatypes(PODs) also satisfy this constraint.
	*/
	template<typename T> concept ios_compatible = is_compatible<T> && os_compatible<T>;

} // namespace aggro


#endif // STREAMCONCEPTS_HPP