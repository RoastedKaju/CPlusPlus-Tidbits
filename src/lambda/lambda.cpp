#include <iostream>
#include <functional>

// ## means if no arguments are provided remove the comma after object parameter
#define BIND_FUNC(func, object, ...) std::bind(func, object, ##__VA_ARGS__)

void free_func() {
    std::printf("Free function.\n");
}

class Greeter {
public:
    void member_func() { std::printf("Member function.\n"); }
    void member_func_param(const char *name) { std::printf("Member function param: %s\n", name); }
};

int main() {
    std::printf("[Raw function pointers]\n");
    // function pointer
    void (*func_pointer)() = &free_func;
    func_pointer();

    // member function example
    Greeter greeter{};
    // to call a member function you must specify the type
    void (Greeter::*func)() = &Greeter::member_func;
    void (Greeter::*func_arg)(const char *name) = &Greeter::member_func_param;

    (greeter.*func)();
    (greeter.*func_arg)("Haris");

    std::printf("[Binding functions example]\n");

    // using bind
    auto bound_func = std::bind(&Greeter::member_func, greeter);
    auto bound_func_arg = std::bind(&Greeter::member_func_param, greeter, std::placeholders::_1);
    auto bound_macro = BIND_FUNC(&Greeter::member_func, greeter);
    auto bound_macro_arg = BIND_FUNC(&Greeter::member_func_param, greeter, std::placeholders::_1);

    bound_func();
    bound_func_arg("Haris");
    bound_macro();
    bound_macro_arg("Haris");

    std::printf("[Lambda examples]\n");
    auto lambda_func = [&greeter] { greeter.member_func(); };
    auto lambda_func_arg = [&greeter](const char *name) { greeter.member_func_param(name); };
    lambda_func();
    lambda_func_arg("Haris");

    return 0;
}
