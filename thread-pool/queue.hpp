#pragma once

#include <queue>
#include <optional>
#include <lines/std/condvar.hpp>
#include <lines/std/mutex.hpp>

template <class T>
class MPMCBlockingUnboundedQueue {
public:
    void Push(T elem) {
        {
            std::scoped_lock guard(lock_);
            if (closed_) {
                return;
            }
            queue_.push(std::move(elem));
        }
        condvar_.NotifyOne();
    }

    std::optional<T> Pop() {
        std::unique_lock guard(lock_);
        while (queue_.empty() && !closed_) {
            condvar_.Wait(guard);
        }

        if (queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    void Close() {
        {
            std::scoped_lock guard(lock_);
            closed_ = true;
        }
        condvar_.NotifyAll();
    }

private:
    std::queue<T> queue_;
    lines::Condvar condvar_;
    lines::Mutex lock_;
    bool closed_ = false;
};
