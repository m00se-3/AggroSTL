#include "array.hpp"
#include <iostream>
#include <string>

int main()
{
    aggro::dynarr<std::string> fs;
    aggro::array<int, 5> is = { 1, 2, 3, 4, 5 };

    aggro::array<int, 5> other = { 6, 7, 8, 9, 0 };

    other = is;
    
    fs = { "Hello", "World" };

    std::cout << is << '\n';
    std::cout << other << '\n';
    std::cout << fs << '\n';
}