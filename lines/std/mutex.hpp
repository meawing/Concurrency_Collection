#pragma once

#include <mutex>

#ifdef LINES_THREADS

namespace lines {

class Mutex {
public:
    void Lock();
    bool TryLock();
    void Unlock();

    void lock() {  // NOLINT
        Lock();
    }
    void unlock() {  // NOLINT
        Unlock();
    }

private:
    std::mutex lock_;
};

}  // namespace lines

#else

#include <lines/sync/wait_queue.hpp>

namespace lines {

class Mutex {
public:
    ~Mutex();
    void Lock();
    bool TryLock();
    void Unlock();

    void lock() {  // NOLINT
        Lock();
    }
    void unlock() {  // NOLINT
        Unlock();
    }

private:
    WaitQueue fibers_;
    Fiber* owner_ = nullptr;
};

}  // namespace lines

#endif
