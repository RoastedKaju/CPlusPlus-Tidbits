#include <iostream>

// Problem: Reverse a Null-Terminated String In-Place Using Only Pointer Arithmetic
// Signature: void reverse_string(char* s);

char str[] = "Hello";

void reverse_string(char *s) {
    char *start = s;
    char *end = s;

    while (*end != '\0') {
        end++;
    }

    --end;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}

int main() {
    reverse_string(str);
    std::cout << str << std::endl;
    return EXIT_SUCCESS;
}
