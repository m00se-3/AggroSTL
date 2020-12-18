#include <iostream>

import Arrays;

template<typename T>
void printArray(const aggro::dyArray<T>& list)
{
	for (auto& i : list)
	{
		std::cout << i << "\n";
	}

	std::cout << std::endl;
}

template<typename T, size_t N>
void printArray(const aggro::stArray<T, N>& list)
{
	for (auto& i : list)
	{
		std::cout << i << "\n";
	}

	std::cout << std::endl;
}

int main()
{
	aggro::stArray<float, 5> numbers;
	aggro::dyArray<int> nums2;

	nums2.pushBack(2);
	nums2.pushBack(3);
	nums2.pushBack(4);
	nums2.pushBack(5);
	nums2.pushBack(6);
	nums2.pushBack(7);

	printArray(nums2);

	nums2.popBack();

	nums2.clear();

	nums2 = { 5, 4, 3, 2, 1 };

	printArray(nums2);

	numbers = { 0.5f, 1.f, 2.2f, 3.f, 4.7f };

	printArray(numbers);
	
}