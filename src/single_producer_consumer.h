#ifndef SINGLE_PRODUCER_CONSUMER_H
#define SINGLE_PRODUCER_CONSUMER_H

#include <iostream>
#include <queue>
#include <array>

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
                std::cout << "The queue is full, cannot push.\n";
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
                std::cout << "The queue is empty, cannot pop.\n";
                return std::nullopt;
            }

            T value = buffer[currentHead];

            // update the head
            head.store(increment(currentHead), std::memory_order_release);

            return value;
        }

    private:
        void increment(const int index)
        {
            return (index + 1) % Size;
        }

    private:
        std::array<T, Size> buffer;

        alignas(64) std::atomic<size_t> head;
        alignas(64) std::atomic<size_t> tail;
    };

    void runSingleProducerConsumerExample()
    {
    }
}

#endif