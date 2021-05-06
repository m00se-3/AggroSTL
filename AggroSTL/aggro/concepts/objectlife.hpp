#ifndef OBJECTLIFE_HPP
#define OBJECTLIFE_HPP

#include <concepts>

namespace aggro
{
    template<typename T>
    concept destructible = std::destructible<T>;
} // namespace aggro


#endif // OBJECTLIFE_HPP