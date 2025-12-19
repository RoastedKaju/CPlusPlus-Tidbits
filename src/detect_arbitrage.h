#ifndef DETECT_ARBITRAGE_H
#define DETECT_ARBITRAGE_H

#include <algorithm>

/// <summary>
/// Determine whether any contiguous subarray has a positive total return.
/// returns = [ -120 -30 40 80 -10 ]
/// 40 + 80 = 120
/// This is a disguised version of maximum subarray sum (Kadane’s algorithm), which quants often frame in PnL terms.
/// The brute force method of this is to loop over every sub array and collect the sum
/// </summary>
namespace arbitrage
{
    int arr[] = { -120, -30, 40, 80, -10 };

    bool HasPositivePnL(const int* returns, int n)
    {
        int result = *(returns);
        int max_ending = *(returns);

        for (int i = 1; i < n; i++)
        {
            max_ending = std::max(*(returns + i), (*(returns + i) + max_ending));

            result = std::max(result, max_ending);
        }

        return result > 0;
    }
}

#endif // !DETECT_ARBITRAGE_H

