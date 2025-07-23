#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    // Disable copy and assignment
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // Enqueue an item
    void enqueue(T item) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(item));
        }
        _cond.notify_one();
    }

    // Blocking dequeue
    T dequeue() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this] { return !_queue.empty(); });

        T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    // Non-blocking dequeue (so you can periodically check while busy doing other work)
    std::optional<T> try_dequeue() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty()) return std::nullopt;

        T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _cond;
    std::queue<T> _queue;
};