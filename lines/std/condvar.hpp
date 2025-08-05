#pragma once

#include <condition_variable>

#ifdef LINES_THREADS

namespace lines {

class Condvar {
public:
    template <class Lockable>
    void Wait(Lockable& lock) {
        Injection();
        condvar_.wait(lock);
        Injection();
    }

    void NotifyOne();
    void NotifyAll();

private:
    void Injection();

private:
    std::condition_variable_any condvar_;
};

}  // namespace lines

#else

#include <lines/sync/wait_queue.hpp>

namespace lines {

class Condvar {
public:
    ~Condvar();

    template <class Lockable>
    void Wait(Lockable& lock) {
        StartWait();
        lock.unlock();
        Suspend();
        lock.lock();
        EndWait();
    }

    void NotifyOne();
    void NotifyAll();

private:
    void StartWait();
    void Suspend();
    void EndWait();

private:
    WaitQueue fibers_;
};

}  // namespace lines

#endif
