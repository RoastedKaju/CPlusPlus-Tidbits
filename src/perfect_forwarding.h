#pragma once

#include <iostream>
#include <vector>
#include <string>

// Before starting lets revise l-values and r-values
// anything you can take an address of by using '&' is an l-value
// for-example int x = 50; and you can also have an l-value reference int& refX = x;
// You can than print out their results by using &x or &refX
// With an r-value you cannot do that, i.e. &50 is not allowed.

namespace perfect_forwarding
{
	struct Object
	{
		int field;
	};

	// This is not allowed
	// Because there is ambugity here
	// we don't know if we want to call the value or reference version
	//void func(Object x)
	//{
	//	std::cout << "Object pass by value func(Object)" << std::endl;
	//}
	// But hey the signatures are different shouldn't they overload?
	// Nope, we at the end don't know if the value we are passing is by ref or not
	void func(Object& x)
	{
		std::cout << "Object pass by ref func(Object&)" << std::endl;
	}

	// you can have a r-value overload, but it is still ambiguous
	void func(Object&& x)
	{
		std::cout << "R-value object pass func(Object&&)" << std::endl;
	}
	// As long as the pass by value exists, it will always hijack the other two versions making things ambigious

	// We can use template to make a wrapper for these functions
	// but with a standard template type deduction it removes the reference qualifier making both versions call the pass by reference even if we moved
	template<typename T>
	void funcWrapper(T x)
	{
		func(x);
	}

	// To fix the issue above and preserve the reference we can use forwarding
	// Also having a templated type T before && doesn't make it r-value it makes it universal reference
	template<typename T>
	void funcWrapperPerfect(T&& x)
	{
		func(std::forward<T>(x));
	}


	void perfectForwardingExample()
	{
		Object x{ 5 };

		func(x);
		func(std::move(x));

		// Wrapper version
		std::cout << "Basic template Wrapper Version" << std::endl;
		Object y{ 5 };
		funcWrapper(y);
		funcWrapper(std::move(y)); // Will still call the pass by ref not r-value because template itself doesn't perfect forward
	
		// Perfect version
		std::cout << "Perfect template Wrapper Version" << std::endl;
		Object z{ 5 };
		funcWrapperPerfect(z);
		funcWrapperPerfect(std::move(z));
	}
}