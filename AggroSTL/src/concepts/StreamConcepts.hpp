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
	template<typename T> concept OStreamCompatible = requires (std::ostream os, T type)
	{
		{ os << type } -> std::same_as<std::ostream&>;
	};

    /*
		This concept is satisfied if an overload exists for the istream(>>) operator.
		Plain Old Datatypes(PODs) also satisfy this constraint.
	*/
	template<typename T> concept IStreamCompatible = requires (std::istream is, T type)
	{
		{ is << type } -> std::same_as<std::istream&>;
	};

	/*
		This concept is satisfied if both stream operators (<< and >>) are implemented.
		Plain Old Datatypes(PODs) also satisfy this constraint.
	*/
	template<typename T> concept IOStreamCompatible = IStreamCompatible<T> && OStreamCompatible<T>;

} // namespace aggro


#endif // STREAMCONCEPTS_HPP