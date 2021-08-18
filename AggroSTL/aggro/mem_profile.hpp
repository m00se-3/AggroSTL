#ifndef MEM_PROFILE_HPP
#define MEM_PROFILE_HPP
#include <iostream>

namespace aggro
{
    /*
        This struct is used to keep track of memory allocations and deallocations in order to detect memory leaks.
        All test functions in the test suite require you to pass in an object of type head_counter and execute it
        via the macro MEM_CHECK.
    */
    struct heap_counter
    {
        inline static size_t mem_alloc = 0u;
        inline static size_t mem_delete = 0u;

        inline static void add(size_t bytes)
        {
            mem_alloc += bytes;
        }

        inline static void remove(size_t bytes)
        {
            mem_delete += bytes;
        }

        heap_counter() = default;
        ~heap_counter()
        {
            std::cout << mem_alloc << " bytes of memory allocated.\n" << mem_delete << " bytes deallocated.\n\n";
            mem_alloc = 0u;
            mem_delete = 0u;
        }
    };

} // namespace aggro

#ifdef AGGRO_MEMORY_PROFILE
/*
    Override global new and delete operators.
*/

[[nodiscard]] inline void* operator new(size_t size)  //Global single object
{
    aggro::heap_counter::add(size);
    return malloc(size);
}

[[nodiscard]] inline void* operator new[](size_t size)    //Global object array
{
    aggro::heap_counter::add(size);
    return malloc(size);
}

inline void operator delete(void* ptr, size_t size) //Global single object delete
{
    aggro::heap_counter::remove(size);
    free(ptr);
}

inline void operator delete[](void* ptr, size_t size)   //Global object array delete
{
    aggro::heap_counter::remove(size);
    free(ptr);
}

#define MEM_CHECK(func) \
std::cout << "Function name " << #func << ":\n";\
func(aggro::heap_counter());

#else
#define MEM_CHECK(func) 

#endif //AGGRO_MEMORY_PROFILE

#endif // MEM_PROFILE_HPP