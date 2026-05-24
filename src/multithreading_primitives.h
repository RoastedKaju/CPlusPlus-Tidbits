#ifndef MULTITHREADING_PRIMITIVES_H
#define MULTITHREADING_PRIMITIVES_H

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <map>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace multithreading_primitives
{
    // Simple function that runs on a thread and prints something
    void helloThread()
    {
        std::cout << "Hello from another thread " << std::this_thread::get_id() << std::endl;
    }

    // Critial section and race condition example
    // suppose we have a DNS database where we read and write using threads
    // multiple threads can read at the same time but only one thread can write
    class DnsDatabase
    {
    private:
        std::map<std::string, std::string> dnsMap;
        mutable std::shared_mutex db_mutex;

    public:
        // Read operation, uses a shared lock
        // multiple threads can call this at the same time
        std::string lookUp(const std::string &domain) const
        {
            std::shared_lock<std::shared_mutex> lock(db_mutex);

            auto itr = dnsMap.find(domain);
            if (itr != dnsMap.end())
            {
                return itr->second;
            }
            return "NXDOMAIN";
        }

        // Write operation, uses unique lock
        // this blocks all readers and other writers until it is done
        void update(const std::string &domain, const std::string &ip)
        {
            std::unique_lock<std::shared_mutex> lock(db_mutex);

            dnsMap[domain] = ip;
            std::cout << "[WRITER] Updated " << domain << " to " << ip << '\n';
        }
    };

    // simulates the time it takes to read database and return data
    void simulateRead(const DnsDatabase &db, const std::string &domain, int id)
    {
        for (int i = 0; i < 3; ++i)
        {
            std::string ip = db.lookUp(domain);
            std::cout << "[Reader #" << id << "] Looked up " << domain << " -> " << ip << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void simulateDNSexample()
    {
        DnsDatabase db;

        // write
        db.update("google.com", "1");
        db.update("yahoo.com", "2");

        // spawn 3 readers
        std::vector<std::thread> threads;
        threads.push_back(std::thread(&simulateRead, std::ref(db), "google.com", 001));
        threads.push_back(std::thread(&simulateRead, std::ref(db), "yahoo.com", 002));
        threads.push_back(std::thread(&simulateRead, std::ref(db), "google.com", 003));
        threads.push_back(std::thread(&simulateRead, std::ref(db), "wikipedia.com", 004));

        // simulate a write while reading is still in progress
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        threads.push_back(std::thread(&DnsDatabase::update, &db, "wikipedia.com", "5"));

        for (auto &t : threads)
        {
            t.join();
        }
    }

    void exampleDriver()
    {
        // Example 1
        std::cout << "Hardware threads " << std::thread::hardware_concurrency() << std::endl;
        std::cout << "Main thread ID " << std::this_thread::get_id() << std::endl;

        std::thread t1(&helloThread);
        t1.join();

        // Example 2
        simulateDNSexample();
    }
}

#endif