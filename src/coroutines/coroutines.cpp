#include <coroutine>
#include <exception>
#include <iostream>

template<typename T>
struct Task
{
    struct promise_type
    {
        T value{};
        std::exception_ptr exception;

        Task get_return_object()
        {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Use suspend_never here to compute the value immediately when the coroutine is called
        // Suspend_always would suspend the coroutine and require an explicit resume to compute the value
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T v) noexcept { value = v; }
        void unhandled_exception() noexcept { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> coro;

    explicit Task(std::coroutine_handle<promise_type> h) : coro(h) {}
    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    Task(Task &&other) noexcept : coro(other.coro) { other.coro = nullptr; }

    Task &operator=(Task &&other) noexcept
    {
        if (this != &other)
        {
            if (coro)
            {
                coro.destroy();
            }
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    ~Task()
    {
        if (coro)
        {
            std::cout << "[Task] Destroying existing coroutine" << std::endl;
            coro.destroy();
        }
    }

    T result()
    {
        if (!coro.done())
        {
            coro.resume(); // run coroutine now
        }
        return coro.promise().value;
    }
};

// Example
Task<int> compute()
{
    co_return 42;
}

template<typename T>
struct Generator
{
    struct promise_type
    {
        T value{};
        std::exception_ptr exception;

        Generator get_return_object()
        {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Lazy start
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        std::suspend_always yield_value(T v) noexcept
        {
            value = v;
            return {};
        }
        void unhandled_exception() noexcept { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> coro;

    explicit Generator(std::coroutine_handle<promise_type> h) : coro(h) {}
    Generator(const Generator &) = delete;
    Generator &operator=(const Generator &) = delete;

    Generator(Generator &&other) noexcept : coro(other.coro)
    {
        other.coro = nullptr;
    }

    Generator &operator=(Generator &&other) noexcept
    {
        if (this != &other)
        {
            if (coro)
            {
                coro.destroy();
            }
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    ~Generator()
    {
        if (coro)
        {
            std::cout << "[Generator] Destroying existing coroutine" << std::endl;
            coro.destroy();
        }
    }

    T result()
    {
        if (!coro.done())
        {
            coro.resume(); // run coroutine now
        }
        if (coro.promise().exception)
        {
            std::rethrow_exception(coro.promise().exception);
        }
        return coro.promise().value;
    }
};

Generator<int> ComputeGenerator()
{
    for (int i = 0; i < 5; ++i)
    {
        co_yield i;
    }
}

int main()
{
    auto task = compute();
    std::cout << "Result: " << task.result() << std::endl;

    auto generator = ComputeGenerator();
    std::cout << generator.result() << std::endl; // Get first value
    std::cout << generator.result() << std::endl; // Get second value
    return EXIT_SUCCESS;
}
