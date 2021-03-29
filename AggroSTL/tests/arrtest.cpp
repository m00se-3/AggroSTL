#include "Arrays.hpp"
#include <iostream>
#include <string>

int main()
{
    aggro::DyArray<float> fs;
    aggro::StArray<int, 5> is = { 1, 2, 3, 4, 5 };

    aggro::StArray<int, 5> other = { 6, 7, 8, 9, 0 };

    other = is;
    
    fs = { 2.f, 3.f, 5.f };

    std::cout << is << '\n';
    std::cout << other << '\n';
    std::cout << fs << '\n';
}