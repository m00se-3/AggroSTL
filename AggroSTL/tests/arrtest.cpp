#define AGGRO_MEMORY_PROFILE
#include "array.hpp"
#include "mem_profile.hpp"
#include <string>


static void test_static_array([[maybe_unused]] aggro::heap_counter h)
{
    aggro::array<int, 5> is = { 1, 2, 3, 4, 5 };
    aggro::array<int, 5> other = { 6, 7, 8, 9, 0 };

    auto comp1 = is.at(2);
    auto comp2 = other.at(4);

    if(comp1 && comp2)
    {
        const char* res = (comp2 == comp1) ? "are" : "are not";

    }
}

static void test_dynamic_array([[maybe_unused]] aggro::heap_counter h)
{
    aggro::darray<std::string> fs;
    
    fs.expand_factor = 0.5f;
    
    fs = { "Hello", "World" };


    fs.emplace_back("Escamo!!");

    auto check = fs.at(6);

    auto ch = fs.at(2);

    if(ch)
    {
        *ch = "Charlie Brown!";
    }

    fs.erase(fs.begin() + 1, fs.begin() + 2);
    fs.shrink_to_fit();
}


int main()
{
    MEM_CHECK(test_static_array)
    MEM_CHECK(test_dynamic_array)

}