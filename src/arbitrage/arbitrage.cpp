#include <iostream>

/**
 * Determine whether any contiguous subarray has a positive total return.
 * returns = [ -120 -30 40 80 -10 ]
 * 40 + 80 = 120
 * This is a disguised version of maximum subarray sum (Kadane’s algorithm), which quants often frame in PnL terms.
 * The brute force method of this is to loop over every sub array and collect the sum
 */

bool has_positive_pnl(const int *data, const int size) {
    int result = *data;
    int max = *data;

    for (auto i = 1; i < size; i++) {
        max = std::max(*(data + i), (*(data + i) + max));
        result = std::max(result, max);
    }

    return result > 0;
}

int main() {
    constexpr int array[] = {-120, -30, 40, 80, -10};

    printf("Has positive PnL: %d", has_positive_pnl(array, 5));

    return EXIT_SUCCESS;
}
