#pragma once

#include <atomic>

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>

// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
inline void FutexWait(int* value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value (-1 wakes all)
inline void FutexWake(int* value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE, count, nullptr, nullptr, 0);
}

// An atomic_compare_exchange wrapper with semantics expected by
// mutex - return the old value stored in the atom.
int CmpxchWrapper(std::atomic<int>* state, int expected, int desired) {
    int* exp = &expected;
    std::atomic_compare_exchange_strong(state, exp, desired);
    return *exp;
}

class Mutex {
public:
    Mutex() : state_(0) {
    }

    void Lock() {
        int old_state = CmpxchWrapper(&state_, 0, 1);
        // If the lock was previously unlocked, there's nothing else for us to do.
        // Otherwise, we'll probably have to wait.
        if (old_state != 0) {
            do {
                // If the mutex is locked, we signal that we're waiting by setting the
                // atom to 2. A shortcut checks is it's 2 already and avoids the atomic
                // operation in this case.
                if (old_state == 2 || CmpxchWrapper(&state_, 1, 2) != 0) {
                    // Here we have to actually sleep, because the mutex is actually
                    // locked. Note that it's not necessary to loop around this syscall;
                    // a spurious wakeup will do no harm since we only exit the do...while
                    // loop when state_ is indeed 0.
                    FutexWait(reinterpret_cast<int*>(&state_), 2);
                }
                // We're here when either:
                // (a) the mutex was in fact unlocked (by an intervening thread).
                // (b) we slept waiting for the atom and were awoken.
                //
                // So we try to lock the atom again. We set teh state to 2 because we
                // can't be certain there's no other thread at this exact point. So we
                // prefer to err on the safe side.
            } while ((old_state = CmpxchWrapper(&state_, 0, 2)) != 0);
        }
    }

    void Unlock() {
        if (state_.fetch_sub(1) != 1) {
            state_.store(0);
            FutexWake(reinterpret_cast<int*>(&state_), 1);
        }
    }

private:
    std::atomic<int> state_;
    // 0: unlocked
    // 1: locked, no waiters
    // 2: locked, there are waiters
};
