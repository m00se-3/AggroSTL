#ifndef MEM_PROFILE_HPP
#define MEM_PROFILE_HPP

namespace aggro
{
    
} // namespace aggro

/*
    Override global new and delete operators.
*/

[[nodiscard]] inline constexpr void* operator new(size_t size)  //Global single object
{
    return ::operator new(size);
}

[[nodiscard]] inline constexpr void* operator new[](size_t size)    //Global object array
{
    return ::operator new[](size);
}

inline constexpr void operator delete(void* ptr, size_t size) //Global single object delete
{
    ::operator delete(ptr, size);
}

inline constexpr void operator delete[](void* ptr, size_t size)   //Global object array delete
{
    ::operator delete[](ptr, size);
}


#endif // MEM_PROFILE_HPP