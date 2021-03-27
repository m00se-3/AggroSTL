#include "Arrays.hpp"
#include <iostream>
#include <string>


int main()
{
    aggro::StArray<std::string, 2> fs = { "Hello", "World" };
    aggro::StArray<int, 5> is = { 1, 2, 3, 4, 5 };

    std::cout << is << '\n';
}