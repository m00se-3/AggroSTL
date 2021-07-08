#include "array.hpp"
#include <iostream>
#include <string>

int main()
{
    aggro::darray<std::string> fs;
    aggro::array<int, 5> is = { 1, 2, 3, 4, 5 };

    aggro::array<int, 5> other = { 6, 7, 8, 9, 0 };

    other = is;
    
    fs = { "Hello", "World" };
    fs.emplace_back("Escamo!!");

    std::cout << is << '\n';
    std::cout << other << '\n';
    std::cout << fs << '\n';

    auto check = fs.at(6);

    if(!check)
        std::cout << "Index 6 does not exist for string dynarr.\n";

    auto check1 = other.at(3);

    if(check1)
        std::cout << "The value at index 3 of other array is " << *check1 << ".\n";

    auto comp1 = is.at(2);
    auto comp2 = other.at(4);

    if(comp1 && comp2)
    {
        const char* res = (comp2 == comp1) ? "are" : "are not";

        std::cout << comp1.value_or(500) << " and " << comp2.value() << " " << res << " equal.\n";
    }

}