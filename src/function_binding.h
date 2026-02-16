#pragma once

#include <iostream>
#include <functional>

#define BIND_FUNC(func, object, ...) std::bind(func, object, ##__VA_ARGS__)

namespace fb
{
	// Free function
	void hello() {
		std::cout << "Hello, world!\n";
	}

	// Class with a member function
	class Greeter
	{
	public:
		void sayHello() { std::cout << "Hello from greeter!\n"; }
		void sayName(const char* name) { std::cout << name << std::endl; }
	};

	void functionBindingExamples() {
		void (*funcPtr)() = &hello;  // function pointer to hello
		funcPtr();                   // calls hello()

		Greeter g; // Greeter object
		void (*fp1)() = &hello; // Works as hello() is a free function
		// void (*fp2)() = &Greeter::sayHello; // won't work as sayHello is a member function
		void(Greeter:: * fp3)() = &Greeter::sayHello; // works now but this will only work for this class (as it needs an object of 'this' Greeter
		// A member function pointer expects a function with an implicit this parameter (the object).

		// std::bind workaround
		auto boundFunc = std::bind(&Greeter::sayHello, &g); // Function address, function params
		auto boundFunc2 = std::bind(&Greeter::sayName, &g, std::placeholders::_1);
		auto boundMacro1 = BIND_FUNC(&Greeter::sayHello, &g);
		auto boundMacro2 = BIND_FUNC(&Greeter::sayName, &g, std::placeholders::_1);
		//boundFunc(); // call it
		//boundFunc2("White");
		boundMacro1();
		boundMacro2("White");

		// Lambda the modern and easy way
		auto lamb1 = [&g]() { g.sayHello(); };
		auto lamb2 = [&g](const char* name) { g.sayName(name); };
		lamb1();
		lamb2("Walter");
	}
}
