#ifndef SINGLE_PRODUCER_CONSUMER_H
#define SINGLE_PRODUCER_CONSUMER_H

#include <iostream>
#include <queue>
#include <array>
#include <thread>
#include <optional>
#include <cstring>
#include <chrono>
#include <atomic>

namespace single_producer_consumer
{
    template <typename T, size_t Size>
    class SPSCQueue
    {
    public:
        bool push(const T &value)
        {
            // Relaxed because we don't care about ordering, only atomic access
            const size_t currentTail = tail.load(std::memory_order_relaxed);
            const size_t nextTail = increment(currentTail);

            // Ensures you see everything written before the pair is released
            if (nextTail == head.load(std::memory_order_acquire))
            {
                return false;
            }

            buffer[currentTail] = value;

            // update the tail index
            // Ensure everything you wrote is visible before publishing
            tail.store(nextTail, std::memory_order_release);

            return true;
        }

        std::optional<T> pop()
        {
            const size_t currentHead = head.load(std::memory_order_relaxed);

            if (currentHead == tail.load(std::memory_order_acquire))
            {
                return std::nullopt;
            }

            T value = buffer[currentHead];

            // update the head
            head.store(increment(currentHead), std::memory_order_release);

            return value;
        }

    private:
        size_t increment(const size_t index) const
        {
            return (index + 1) % Size;

            // Use this power of 2 optimization if your queue is something like 1024
            // this is a very common optimization technique
            // return (index + 1) & (Size - 1);
        }

    private:
        std::array<T, Size> buffer;

        alignas(64) std::atomic<size_t> head{0};
        alignas(64) std::atomic<size_t> tail{0};
    };

    struct LogMessage
    {
        // Array over std::string because string can sometimes be heap allocated
        // which is heavier, so best use fixed size arrays
        char text[128];
    };

    class Logger
    {
    public:
        void log(const char *msg)
        {
            LogMessage log{};

            // this will copy the string from 'msg' into log.text safely with null termination
            std::snprintf(log.text, sizeof(log.text), "%s", msg);

            while (!queue.push(log))
            {
                // queue full
                std::this_thread::yield();

                // Using yield is not optimal all the time because it still uses CPU
                // better option would be to implement spinning or sleep if sleep is something that you can afford to do
                // same goes for using yeild in log function below
            }
        }

        void processLogs()
        {
            while (running)
            {
                auto msg = queue.pop();

                if (msg)
                {
                    std::cout << msg->text << "\n";
                }
                else
                {

                    std::cout << "Nothing to log, Queue is empty" << "\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }

        void stop()
        {
            // This assumes that we don't care if the logger still has logs in queue
            // when we shut down, we are force terminating.
            // a better production ready solution will be to let the logger complete than shutdown
            running = false;
        }

    private:
        SPSCQueue<LogMessage, 1024> queue;

        std::atomic<bool> running = true;
    };

    void runSingleProducerConsumerExample()
    {
        Logger logger;

        // thread starting, will now process any log that is pushed into the log queue
        std::thread loggerThread(&Logger::processLogs, &logger);

        // push into log queue
        for (int i = 0; i < 3; i++)
        {
            logger.log("Player updated");
        }

        // Lets say user exits the program after n-number of seconds
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // Tell logger to stop after some time, simulates application shutdown
        logger.stop();
        loggerThread.join();

        std::cout << "Program stopped, logger thread has joined and finished as well" << "\n";
    }
}

#endif