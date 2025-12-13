#ifndef REMOVE_SPACES_H
#define REMOVE_SPACES_H

/// <summary>
/// Using only pointer arithmetic remove all spaces from a string
/// Search pointer read, write method to solve this.
/// </summary>
namespace remove_spaces
{
	char str[] = "  h e l lo  w or ld  ";

	void RemoveSpaces(char* s)
	{
        char* read = s;
        char* write = s;

        while (*read != '\0')
        {
            if (*read != ' ')
            {
                *write = *read;
                write++;
            }
            read++;
        }

        *write = '\0';

		std::cout << s << std::endl;
	}
}


#endif // !REMOVE_SPACES_H
