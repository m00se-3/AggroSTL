#include "Arrays.hpp"
#include <iostream>


int main()
{
    aggro::DyArray<float> fs = { 2.3f, 5.7f };
    aggro::StArray<int, 5> is = { 1, 2, 3, 4, 5 };

    std::cout << fs << '\n';
}