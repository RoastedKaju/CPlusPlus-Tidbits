#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace variadic
{
	// In variadic templates you must have a base case and a use ... to in a way call the arguments
	template<typename... Args>
	auto SumAll(Args... args)
	{
		return (args + ...); // Notice the ...
	}

	// Another way to do this is with perfect forwarding
	template<typename... Args>
	constexpr auto SumAllPerfect(Args&&... args)
	{
		return (args + ...);
	}

	// ... is a pack expression
	// using ... with templates is called variadic templates
	// ... was even in C you can use it in macros, lambdas and other functions (but without templates those are runtime)
	// folding expression is a modern C++ 17 feature that means using it in expression like the one you see in return
	// (args + ...) Unary right fold
	// (... + args) Unary left fold
	// (0 + ... + args) Binary left fold
	// (args + ... + 0) Binary right fold

	void variadicExample()
	{
		std::cout << SumAll(1, 2, 3, 4) << std::endl;
		std::cout << SumAllPerfect(1, 2, 3, 4) << std::endl;
		std::cout << SumAllPerfect<float>(1, 2, 3, 4.7f) << std::endl;

	}
}