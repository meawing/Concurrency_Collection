#pragma once

#include <lines/std/atomic.hpp>
#include <lines/fibers/api.hpp>

class SpinLock {
public:
    void Lock() {
        while (true) {
            // Try to acquire the lock by exchanging false->true
            bool expected = false;
            if (locked_.compare_exchange_strong(expected, true)) {
                return;
            }
            // If failed to acquire, yield to other fibers
            lines::Yield();
        }
    }

    void Unlock() {
        locked_.store(false);
    }

private:
    lines::Atomic<bool> locked_{false};
};
