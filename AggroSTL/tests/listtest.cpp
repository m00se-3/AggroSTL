#include "list.hpp"
#include <iostream>
#include <string>


int main()
{
    //plain data test

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

    aggro::dlist<std::string> words = { "one", "two", "three", "four" };

    std::cout << words << "\n";

    words.insert(words.begin() + 3, "cat");

    std::cout << words << "\n";

    words.pop_front();

    std::cout << words << "\n";

    words.emplace_front("dog");
    words.emplace_back("fish");

    std::cout << words << "\n";

    words.erase(words.begin() + 2);

    std::cout << words << "\n";
}
