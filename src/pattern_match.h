#ifndef PATTERN_MATCH_H
#define PATTERN_MATCH_H

#include <iostream>

/// <summary>
/// Problem: Find the first Occurence of a sub array using pointer arithmetic
/// Given: Array 'a' with elements: { 3, 5, 1, 2, 8, 1, 2, 3 }, Array 'pattern' with elements: { 1, 2 }
/// Function Signature: int FindPattern(const int* a, int size_a, const int* p, int size_p)
/// On Match return 0
/// On No match return -1
/// </summary>

namespace pattern_match
{
	int arr[8] = { 3, 5, 1, 2, 8, 1, 2, 3 };
	int pattern[2] = { 1, 2 };

	inline int FindPattern(const int* a, int size_a, const int* p, int size_p)
	{
		// Setup starting pattern
		const int* pat_ptr = p;
		int match_count = 0;

		// Loop over array
		for (int i = 0; i < size_a; i++)
		{
			if (*(a + i) == *(pat_ptr))
			{
				match_count++;
				pat_ptr++;

				if (match_count == size_p)
				{
					std::cout << "Match found" << std::endl;
					return 0;
				}
			}
			else
			{
				pat_ptr = p;
				match_count = 0;

				// Check if this new element is overlaping with our pattern
				if (*(a + i) == *pat_ptr)
				{
					match_count++;
					pat_ptr++;
				}
			}
		}

		return -1;
	}
}

#endif // !PATTERN_MATCH_H
