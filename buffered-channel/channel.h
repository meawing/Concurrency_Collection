#pragma once

#include <optional>
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t capacity) : capacity_(capacity), closed_(false) {
    }

    void Push(T elem) {
        std::unique_lock<std::mutex> lock(mutex_);

        // If channel is closed, push has no effect
        if (closed_) {
            return;
        }

        // Wait until there's space in the buffer
        not_full_.wait(lock, [this] { return queue_.size() < capacity_ || closed_; });

        // Check again if channel was closed while waiting
        if (closed_) {
            return;
        }

        queue_.push(std::move(elem));

        // Notify one waiting consumer that data is available
        not_empty_.notify_one();
    }

    std::optional<T> Pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        // Wait until there's data or channel is closed
        not_empty_.wait(lock, [this] { return !queue_.empty() || closed_; });

        // If queue is empty and channel is closed, return empty optional
        if (queue_.empty()) {
            return std::nullopt;
        }

        // Get the front element
        T value = std::move(queue_.front());
        queue_.pop();

        // Notify one waiting producer that space is available
        not_full_.notify_one();

        return value;
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        closed_ = true;

        // Wake up all waiting threads
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    //
    std::queue<T> queue_;
    const size_t capacity_;
    bool closed_;
    std::mutex mutex_;
    std::condition_variable not_empty_;  // Signaled when queue is not empty
    std::condition_variable not_full_;   // Signaled when queue is not full
};
