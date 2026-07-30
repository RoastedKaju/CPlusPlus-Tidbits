#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

void hello_thread()
{
    std::cout << "Hello from another thread: " << std::this_thread::get_id() << std::endl;
}

// DNS database, critical section and race condition example.
// We read and write on this database using threads.
// multiple threads can read at the same time and only thread can write.
class Database
{
public:
    // read operation use shared lock as multiple threads can read at the same time
    std::string look_up(const std::string &domain) const
    {
        std::shared_lock<std::shared_mutex> lock(db_mutex);

        if (const auto itr = dns_map.find(domain); itr != dns_map.end())
        {
            return itr->second;
        }

        return "NXDOMAIN";
    }

    // write operation, use unique lock
    void update(const std::string &domain, const std::string &ip)
    {
        std::unique_lock<std::shared_mutex> lock(db_mutex);

        dns_map[domain] = ip;
        std::cout << "[WRITER] Updated " << domain << " to " << ip << std::endl;
    }

private:
    std::map<std::string, std::string> dns_map;
    mutable std::shared_mutex db_mutex;
};

// simulate the time it takes to read database and return data
static void simulate_read(const Database &db, const std::string &domain, int id)
{
    for (int i = 0; i < 3; ++i)
    {
        std::string ip = db.look_up(domain);
        std::cout << "[Reader # " << id << "] Looked up " << domain << " -> " << ip << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Foo Bar example
void foo(std::mutex &mutex, std::condition_variable &cv, bool &foo_printed)
{
    for (size_t i = 0; i < 100; ++i)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&foo_printed]()
                { return !foo_printed; });
        std::cout << "Foo\n";
        foo_printed = true;
        cv.notify_all();
    }
}

void bar(std::mutex &mutex, std::condition_variable &cv, bool &foo_printed)
{
    for (size_t i = 0; i < 100; ++i)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&foo_printed]()
                { return foo_printed; });
        std::cout << "Bar\n";
        foo_printed = false;
        cv.notify_all();
    }
}

int main()
{
    std::cout << "Total hardware threads: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;

    std::thread t1(&hello_thread);
    t1.join();

    // Simulate database example with DNS
    Database database{};

    database.update("google.com", "1");
    database.update("yahoo.com", "2");

    // Spawn readers
    std::vector<std::thread> threads;
    threads.push_back(std::thread(&simulate_read, std::ref(database), "google.com", 001));
    threads.push_back(std::thread(&simulate_read, std::ref(database), "yahoo.com", 002));
    threads.push_back(std::thread(&simulate_read, std::ref(database), "google.com", 003));
    threads.push_back(std::thread(&simulate_read, std::ref(database), "wikipedia.com", 004));

    // simulate a write when reading is still in progress
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    threads.push_back(std::thread(&Database::update, &database, "wikipedia.com", "5"));

    for (auto &thread: threads)
    {
        thread.join();
    }

    // FooBar example
    std::mutex mtx;
    std::condition_variable cv;
    bool foo_printed = false;
    std::thread foo_thread(&foo, std::ref(mtx), std::ref(cv), std::ref(foo_printed));
    std::thread bar_thread(&bar, std::ref(mtx), std::ref(cv), std::ref(foo_printed));

    foo_thread.join();
    bar_thread.join();

    return EXIT_SUCCESS;
}
