#include <iostream>

// You are given an integer array where: Every Number appears exactly twice, One Number appears once.
// Find the unique number in the list.
// Use XOR, Time should be O(1), no extra space.

int my_list[] = { 4, 1, 2, 1, 2 };

int find_unique(const int* arr, int n)
{
    int result = *arr;
    for (int i = 1; i < n; i++)
    {
        result ^= *(arr + i);
    }

    return result;
}

int main() {
    auto result = find_unique(my_list, 5);
    std::cout << result << std::endl;
    return EXIT_SUCCESS;
}