#pragma once

#include <optional>
#include <condition_variable>
#include <mutex>
#include <new>
#include <stdexcept>

template <class T>
class UnbufferedChannel {
public:
    UnbufferedChannel()
        : closed_(false), has_value_(false), sender_waiting_(false), receiver_waiting_(false) {
    }

    ~UnbufferedChannel() {
        if (has_value_) {
            reinterpret_cast<T*>(storage_)->~T();
        }
    }

    void Push(T value) {
        std::unique_lock<std::recursive_mutex> lock(mutex_);

        // Wait until there's no value in the channel
        not_full_.wait(lock, [this] { return !has_value_ || closed_; });

        // Check again if channel was closed while waiting
        if (closed_) {
            throw std::runtime_error("Cannot push to closed channel");
        }

        // Store the value using placement new
        new (storage_) T(std::move(value));
        has_value_ = true;
        sender_waiting_ = true;

        // Notify all waiting consumers
        not_empty_.notify_all();

        // Wait until a consumer takes the value or channel is closed
        while (has_value_ && !closed_) {
            transaction_complete_.wait(lock);
        }

        // Reset sender waiting state
        sender_waiting_ = false;

    }

    std::optional<T> Pop() {
        std::unique_lock<std::recursive_mutex> lock(mutex_);

        // Wait until there's a value or channel is closed
        receiver_waiting_ = true;
        while (!has_value_ && !closed_) {
            not_empty_.wait(lock);
        }
        receiver_waiting_ = false;

        // If no value and channel is closed, return empty optional
        if (!has_value_) {
            return std::nullopt;
        }

        // Get the value
        T* value_ptr = reinterpret_cast<T*>(storage_);
        std::optional<T> result = std::move(*value_ptr);

        // Destroy the value
        value_ptr->~T();
        has_value_ = false;

        // Notify waiting producers
        not_full_.notify_all();
        transaction_complete_.notify_all();

        return result;
    }

    void Close() {
        std::unique_lock<std::recursive_mutex> lock(mutex_);
        closed_ = true;

        // Wake up all waiting threads
        not_empty_.notify_all();
        not_full_.notify_all();
        transaction_complete_.notify_all();
    }

private:
    std::recursive_mutex mutex_;
    std::condition_variable_any not_empty_;
    std::condition_variable_any not_full_;
    std::condition_variable_any transaction_complete_;
    bool closed_;
    bool has_value_;
    bool sender_waiting_;
    bool receiver_waiting_;

    // Aligned storage for T
    alignas(T) unsigned char storage_[sizeof(T)];
};
