#include <iostream>

char new_destination[3];
char buffer[] = "ABCDE";

void *custom_mem_move(void *dst, const void *src, size_t n) {
    unsigned char *d = static_cast<unsigned char *>(dst);
    const unsigned char *s = static_cast<const unsigned char *>(src);

    if (d > s) {
        std::printf("Backward copy.\n");
        for (int i = n - 1; i >= 0; --i) {
            *(d + i) = *(s + i);
        }
    } else {
        std::printf("Forward copy.\n");
        for (int i = 0; i < n; ++i) {
            *(d + i) = *(s + i);
        }
    }

    return dst;
}

int main() {
    custom_mem_move(new_destination, buffer, 2);
    return EXIT_SUCCESS;
}
