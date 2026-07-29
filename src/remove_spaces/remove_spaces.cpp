#include <iostream>

// Using only pointer arithmetic remove all spaces from a string
// Search pointer read, write method to solve this.
char str[] = "  h e l lo  w or ld  ";

void remove_spaces(char *s) {
    char *read = s;
    char *write = s;

    while (*read != '\0') {
        if (*read != ' ') {
            *write = *read;
            write++;
        }
        read++;
    }

    *write = '\0';

    std::cout << s << std::endl;
}

int main() {
    remove_spaces(str);
    return EXIT_SUCCESS;
}
