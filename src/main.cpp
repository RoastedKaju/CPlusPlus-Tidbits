#include <iostream>

#include "beeper.h"
#include "increment_overload.h"
#include "pattern_match.h"
#include "reverse_string.h"
#include "remove_spaces.h"

int main()
{
	/* Beep after 3 seconds */
	//TheBeeper();

	/* Example overloading of post and pre increment */
	//IncrementOverloadExample a(3);
	//IncrementOverloadExample b(3);

	//std::cout << a++ << std::endl;
	//std::cout << ++b << std::endl;

	//pattern_match::FindPattern(pattern_match::arr, 8, pattern_match::pattern, 2);

	//reverse_string::ReverseString(reverse_string::str);

	//std::cout << reverse_string::str << std::endl;

	remove_spaces::RemoveSpaces(remove_spaces::str);

	return EXIT_SUCCESS;
}