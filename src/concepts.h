#pragma once

#include <string>
#include <concepts>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

///
/// A concept is just a true of false statement, it is like a predicate that returns true or false
/// there are many built in concepts as well.
///

template<typename T> requires std::integral<T>
void printValue(T value)
{
	std::cout << "Value is " << value << std::endl;
}

template<typename T> requires std::integral<T>
bool isEqual(T valueA, T valueB)
{
	std::cout << "Calling the integral version\n";
	return valueA == valueB;
}

// You can also do template specialization with these, same as regular templates
template<typename T> requires std::floating_point<T>
bool isEqual(T valueA, T valueB)
{
	std::cout << "Calling the float version\n";
	return std::fabs(valueA - valueB) < 0.0001f;
}

// Another way to shorten the syntax is this, instead of typename use the concept here
template<std::unsigned_integral T>
bool isEqual(T valueA, T valueB)
{
	std::cout << "Calling the unsigned integral version\n";
	return valueA == valueB;
}

// You can even use auto to get rid of template syntax all together, remember auto is basically making a template and deducing at compile time
bool isEqualAutoVer(std::floating_point auto valueA, std::floating_point auto valueB)
{
	std::cout << "Calling the auto float version\n";
	return std::fabs(valueA - valueB) < 0.0001f;
}

// Let's make our own custom concept that is always going to return true
template<typename T>
concept AlwaysTrue = true;

// Now use it
template<typename T> requires AlwaysTrue<T>
void PrintSomething(T value)
{
	std::cout << value << std::endl;
}

// Or avoid all this template crap and simply use auto
void PrintSomethingAutoVer(AlwaysTrue auto value)
{
	std::cout << value << std::endl;
}

// Time to make another custom concept that combines two of the built in ones
template<typename T>
concept IsFloatORInt = std::floating_point<T> || std::integral<T>;

// Use it
template<typename T> requires IsFloatORInt<T>
void PrintFloatOrInt(T value)
{
	std::cout << value << std::endl;
}

// Make a concept that is similar to floating point but for double
template<typename T>
concept IsDouble = std::same_as<T, double>; // Needs two types

// Use it
void PrintDoubleAutoVer(IsDouble auto value)
{
	std::cout << value << std::endl;
}

// Another example, lets make a concept for small data (size less than 8 bytes) and a fallback for big data
template<typename T>
concept SmallDataExpression = sizeof(T) <= 8; // Size is present during compile time

// Use our concepts
template <typename T> requires SmallDataExpression<T>
void PrintData(T value)
{
	std::cout << "The size of our small data is: " << sizeof(value) << std::endl;
}

// Make one for big data that takes in the value via const reference
template<typename T>
concept BigDataExpression = !SmallDataExpression<T>; // Just NOT the small data predicate

// This is kinda like SFINAE where if one template overload fails to do something have another
template<typename T> requires BigDataExpression<T>
void PrintData(const T& value)
{
	std::cout << "The size of our big data is: " << sizeof(value) << std::endl;
}

// Or you could have done just this
template<typename T> requires (!SmallDataExpression<T>) // Requires need an expression so enclose it in braces
void PrintBigData()
{
	// Print data size
}

void conceptsExample()
{
	printValue(5);
	//printValue(1.0); // Will not work as it does not satisfy the concept expression

	std::cout << std::boolalpha << isEqual(1, 1) << std::endl;
	std::cout << std::boolalpha << isEqual(1.0f, 5.0f) << std::endl;	// This will work as we have a specialization
	std::cout << std::boolalpha << isEqual(1u, 5u) << std::endl;
	std::cout << std::boolalpha << isEqualAutoVer(1.0f, 5.0f) << std::endl; // Auto version

	PrintSomething(10);
	PrintSomethingAutoVer("Hello World!"); // Since the concept expression result is always true we can put pretty much any argument here

	std::cout << "OR based Concepts" << std::endl;
	PrintFloatOrInt(10);
	PrintFloatOrInt(100.0f);
	//PrintFloatOrInt("Haris"); // Will not work as the function template only requries floating or integer

	// Custom double concept
	PrintDoubleAutoVer(1000.0);

	PrintData(23);
	{
		std::unique_ptr<int> somePtr; // Should be 8 bytes
		PrintData(std::move(somePtr));
	}
	std::vector<int> someVec = {1, 3, 4, 5, 6};
	PrintData(someVec); // The size of vector is 32 bytes, doesn't matter how many elements it has
	// The big data one will use the 2nd overload with const reference

}