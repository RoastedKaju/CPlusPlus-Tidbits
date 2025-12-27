#ifndef MAX_DRAWDOWN_H
#define MAX_DRAWDOWN_H

#include <iostream>

namespace max_drawdown
{
	int arr[] = { 100, -20, 30, -150, 10, -5 };

	int MaxDrawdown(const int* pnl, int n)
	{
		int equity = 0;
		int peak = 0;
		int max_dd = 0;

		const int* end = pnl + n;

		while (pnl < end)
		{
			equity += *pnl;

			if (peak < equity)
			{
				peak = equity;
			}
			
			int trough = peak - equity;

			if (trough > max_dd)
			{
				max_dd = trough;
			}

			++pnl;
		}

		return max_dd;
	}
}

#endif