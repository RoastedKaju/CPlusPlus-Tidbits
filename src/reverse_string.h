#ifndef REVERSE_STRING_H
#define REVERSE_STRING_H

/// <summary>
/// Problem: Reverse a Null-Terminated String In-Place Using Only Pointer Arithmetic
/// Signature: void reverseString(char* s);
/// </summary>
namespace reverse_string
{
	char str[] = "Hello";

	void ReverseString(char* s)
	{
		char* start = s;
		char* end = s;
		
		while (*end != '\0')
		{
			end++;
		}

		--end;

		while (start < end)
		{
			char temp = *start;
			*start = *end;
			*end = temp;

			start++;
			end--;
		}
	}
}

#endif // !REVERSE_STRING_H
