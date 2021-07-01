#include "list.hpp"
#include <iostream>
#include <string>


int main()
{
    aggro::slist<int> nums = { 1, 2, 3, 4 };

    std::cout << nums << "\n";

    nums.insert_after(nums.begin() + 3, 99);

    std::cout << nums << "\n";

    nums.pop_front();

    std::cout << nums << "\n";

    nums.push_front(302);

    std::cout << nums << "\n";

    nums.erase_after(nums.begin() + 2);

    std::cout << nums << "\n";

    // strings test

    aggro::slist<std::string> words = { "one", "two", "three", "four" };

    std::cout << words << "\n";

    words.insert_after(words.begin() + 3, "cat");

    std::cout << words << "\n";

    words.pop_front();

    std::cout << words << "\n";

    words.push_front("dog");

    std::cout << words << "\n";

    words.erase_after(words.begin() + 2);

    std::cout << words << "\n";
}
