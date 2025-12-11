#ifndef INCREMENT_OVERLOAD_H
#define INCREMENT_OVERLOAD_H

#include <iostream>
#include <string>

class IncrementOverloadExample
{
public:
	// Ctor
	IncrementOverloadExample(int x) : value(x) {}

	// Increment overload
	// Prefix
	IncrementOverloadExample& operator++()
	{
		++value;
		return *this;
	}

	// Postfix
	IncrementOverloadExample operator++(int)
	{
		IncrementOverloadExample temp = *this;
		++value;
		return temp;
	}

	inline void PrintValue()
	{
		std::cout << "'IncrementOverloadExample' Value : " << value << std::endl;
	}

	// To support streaming out value, friend so it can access private members
	inline friend std::ostream& operator<<(std::ostream& os, const IncrementOverloadExample& obj)
	{
		os << "Value : " << obj.value;
		return os;
	}

private:
	int value;
};

#endif // !INCREMENT_OVERLOAD_H
