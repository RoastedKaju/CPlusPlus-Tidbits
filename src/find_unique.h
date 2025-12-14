#ifndef FIND_UNIQUE_H
#define FIND_UNIQUE_H

/// <summary>
/// You are given an integer array where: Every Number appears exactly twice, One Number appears once.
/// Find the unique number in the list.
/// Use XOR, Time should be O(1), no extra space.
/// </summary>
namespace find_unique
{
	int my_list[] = { 4, 1, 2, 1, 2 };

	int FindUnique(const int* arr, int n)
	{
		int result = *arr;
		for (int i = 1; i < n; i++)
		{
			result ^= *(arr + i);
		}

		return result;
	}
}

#endif // !FIND_UNIQUE_H
