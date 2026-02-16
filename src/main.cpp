#include <iostream>

#include "beeper.h"
#include "increment_overload.h"
#include "pattern_match.h"
#include "reverse_string.h"
#include "remove_spaces.h"
#include "implement_memmove.h"
#include "find_unique.h"
#include "detect_arbitrage.h"
#include "max_drawdown.h"
#include "singleton.h"
#include "crtp_polymorphism.h"
#include "function_binding.h"

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

	//remove_spaces::RemoveSpaces(remove_spaces::str);

	//std::cout << (unsigned char*)impl_memmove::my_memmove(impl_memmove::buffer + 1, impl_memmove::buffer, 4) << std::endl;

	//impl_memmove::my_memmove(impl_memmove::buffer, impl_memmove::buffer + 1, 4);
	//std::cout << (unsigned char*)impl_memmove::buffer << std::endl;

	//impl_memmove::my_memmove(impl_memmove::new_dest, impl_memmove::buffer + 2, 3);
	//std::cout << (unsigned char*)impl_memmove::new_dest << std::endl;

	//std::cout << find_unique::FindUnique(find_unique::my_list, 5) << std::endl;

	//arbitrage::HasPositivePnL(arbitrage::arr, 5);

	//max_drawdown::MaxDrawdown(max_drawdown::arr, 6);

	//singleton::Singleton& my_singleton = singleton::Singleton::GetInstance();

	//my_singleton.DoSomething();

	//std::unique_ptr<crtp_poly::Shape> circle = std::make_unique<crtp_poly::Circle>(2.0f);
	//std::cout << circle->area() << std::endl;

	//std::unique_ptr<crtp_poly::ShapeCRTP<crtp_poly::CircleCRTP>> circleCompilePoly = std::make_unique<crtp_poly::CircleCRTP>(2.0f);
	//std::cout << circleCompilePoly->area() << std::endl;

	fb::functionBindingExamples();

	return EXIT_SUCCESS;
}