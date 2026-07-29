#include <iostream>

class Singleton
{
public:
    // Create a Get Function with thread-safe inline static variable
    static Singleton& GetInstance()
    {
        static Singleton instance;
        return instance;
    }

    // Delete Move and Copy operations
    Singleton(const Singleton&) = delete;

    Singleton(Singleton&&) noexcept = delete;

    Singleton& operator=(const Singleton&) = delete;

    Singleton& operator=(Singleton&&) noexcept = delete;

    inline void do_something()
    {
        std::cout << "Singleton did something\n";
    }

    ~Singleton() = default;

private:
    // Make the constructor private
    Singleton() = default;
};

int main() {
    Singleton::GetInstance().do_something();
    return EXIT_SUCCESS;
}