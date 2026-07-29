#include <iostream>
#include <string>

class OverloadExample {
public:
    explicit OverloadExample(const int x) : value(x) {
    }

    // Increment overload
    // Prefix
    OverloadExample &operator++() {
        ++value;
        return *this;
    }

    // Postfix
    OverloadExample operator++(int) {
        const OverloadExample temp = *this;
        ++value;
        return temp;
    }

    void print() const {
        std::cout << "Value : " << value << std::endl;
    }

    // support streaming out value
    // friend so it can access private members
    friend std::ostream &operator<<(std::ostream &os, const OverloadExample &obj) {
        os << "Value : " << obj.value;
        return os;
    }

private:
    int value;
};

int main() {
    OverloadExample example{12};
    ++example;
    std::cout << example++ << std::endl;

    return EXIT_SUCCESS;
}
