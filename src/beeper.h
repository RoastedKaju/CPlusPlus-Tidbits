#ifndef BEEPER_H
#define BEEPER_H

#include <iostream>
#include <chrono>

inline static void TheBeeper()
{
	// Get the current time point
	auto current_time = std::chrono::system_clock::now();
	std::chrono::duration<double> beep_interval(3.0);
	// Add the duration to the time point
	auto beep_time = current_time + beep_interval;

	while (true)
	{
		if (std::chrono::system_clock::now() > beep_time)
		{
			// Beep
			std::cout << "Beeped - " << '\a' << std::endl;

			// set the timer for next round
			beep_time = std::chrono::system_clock::now() + beep_interval;
		}
	}
}

#endif // !BEEPER_H
