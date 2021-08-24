#define AGGRO_MEMORY_PROFILE
#include "profile.hpp"
#include "list.hpp"
#include <string>
#include <forward_list>
#include <list>

static void test_slist_with_pod([[maybe_unused]] aggro::bench_timer t, [[maybe_unused]] aggro::heap_counter h)
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
}

static void test_dlist_with_strings([[maybe_unused]] aggro::bench_timer t, [[maybe_unused]] aggro::heap_counter h)
{
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

static void test_list_from_empty([[maybe_unused]] aggro::bench_timer t, [[maybe_unused]] aggro::heap_counter h)
{
    aggro::slist<size_t> single;
    aggro::dlist<size_t> dub;

    for(size_t i = 0u; i < 1000u; ++i)
    {
        if(i % 4u == 3u)
        {
            dub.pop_front();
            single.erase_after(single.begin());
        }
        
        dub.emplace_back(i);
        single.emplace_front(i);
    }

}

static void std_list_from_empty([[maybe_unused]] aggro::bench_timer t, [[maybe_unused]] aggro::heap_counter h)
{
    std::forward_list<size_t> single;
    std::list<size_t> dub;

    for(size_t i = 0u; i < 1000u; ++i)
    {
        if(i % 4u == 3u)
        {
            dub.pop_front();
            single.erase_after(single.begin());
        }
        
        dub.emplace_back(i);
        single.emplace_front(i);
    }

}


int main()
{
    
    MEM_CHECK(test_slist_with_pod)
    MEM_CHECK(test_dlist_with_strings)
    MEM_CHECK(test_list_from_empty)
    MEM_CHECK(std_list_from_empty)
    
}
