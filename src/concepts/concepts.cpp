#include <iostream>
#include <vector>
#include <concepts>
#include <array>

/**
 * Concepts are like small predicate functions, that may return true or false.
 * There are many built in concepts that come with the language which you can use.
 */

template<typename T> requires std::integral<T>
void print_value(T value) {
    std::cout << "Value is " << value << '\n';
}

// Different specifications for different overloads
template<typename T> requires std::integral<T>
bool compare(T a, T b) {
    std::cout << "Integral version\n";
    return a == b;
}

template<typename T> requires std::floating_point<T>
bool compare(T a, T b) {
    std::cout << "Floating version\n";
    return a == b;
}

// Another way to shorten the syntax is this
template<std::unsigned_integral T>
bool compare(T a, T b) {
    std::cout << "Unsigned integral version\n";
    return a == b;
}

// There is also an 'auto' version
bool is_equal(std::floating_point auto a, std::floating_point auto b) {
    std::cout << "Floating point auto version\n";
    return a == b;
}

// Custom concepts
template<typename T>
concept always_true = true;

template<typename T> requires always_true<T>
void print_something() {
    std::cout << "Using a custom template.\n";
}

// Make a custom concept that ties two other concepts
template<typename T>
concept is_a_number = std::integral<T> || std::floating_point<T>;

template<typename T> requires is_a_number<T>
void take_number(T value) {
    std::cout << "This can take both an integer and a floating point " << value << '\n';
}

// You can also use the same as standard function helper
template<typename T>
concept is_double = std::same_as<T, double>;

// Just using the auto version
void take_double(is_double auto value) {
    std::cout << "This only works for doubles " << value << '\n';
}

// Now lets make something like SFINAE where if one template function specialization fails
// have another one that runs instead
// Make two templated functions one for small data (Less than 8 bytes) and one for large data
template<typename T>
concept is_small_data = sizeof(T) <= 8;

template<typename T>
concept is_big_data = !is_small_data<T>;

template<typename T> requires is_small_data<T>
void print_data([[maybe_unused]] const T &value) {
    std::cout << "Small data version, size of data is: " << sizeof(value) << '\n';
}

template<typename T> requires is_big_data<T>
void print_data([[maybe_unused]] const T &value) {
    std::cout << "Big data version, size of data is: " << sizeof(value) << '\n';
}

// Now something complex, make a concept for types that have the push-back function
template<typename T>
concept has_push_back = requires(T container, typename T::value_type value)
{
    container.push_back(value);
};

template<typename T> requires has_push_back<T>
void add_to_container(T &container, int value) {
    container.push_back(value);
}

// Challenge: write a concept for containers that have a size function
template<typename T>
concept has_size_function = requires(T container)
{
    container.size();
};

template<has_size_function T>
void print_size_of_container(const T &container) {
    std::cout << "Size of container is " << container.size() << '\n';
}


int main() {
    print_value(18);

    compare(10, 5);
    compare(12.0f, 12.0f);
    compare(5u, 6u);

    is_equal(15.0f, 10.0f);

    // Anything will work on this
    print_something<int>();

    take_number(10);
    take_number(10.0f);

    take_double(100.0);

    print_data(10.0f);
    print_data(100.0l);

    // Size of vector is always 24 bytes as rest of data resides in heap, so it will always trigger big data version
    // 24 bytes on 64 bit and 12 bytes on 32 bit.
    // pointer to data on heap (8 bytes)
    // size field (8 bytes)
    // capacity field (8 bytes)
    std::vector<int> vec{10, 20, 30};
    print_data(vec);

    add_to_container(vec, 20);
    std::cout << "Last item pushed to container is: " << vec.back() << '\n';

    [[maybe_unused]] int array[] = {1, 2, 3};
    constexpr std::array<int, 3> standard_array = {1, 2, 3};

    print_size_of_container(vec);
    print_size_of_container(standard_array);

    return EXIT_SUCCESS;
}
