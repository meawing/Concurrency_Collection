#include <lines/std/mutex.hpp>
#include <lines/fault/injection.hpp>

#include <libassert/assert.hpp>

#ifdef LINES_THREADS

namespace lines {

void Mutex::Lock() {
    InjectFault();
    lock_.lock();
    InjectFault();
}

bool Mutex::TryLock() {
    InjectFault();
    bool result = lock_.try_lock();
    InjectFault();
    return result;
}

void Mutex::Unlock() {
    InjectFault();
    lock_.unlock();
    InjectFault();
}

}  // namespace lines

#else

#include <lines/fibers/scheduler.hpp>

namespace lines {

Mutex::~Mutex() {
    ASSERT(!owner_);
    ASSERT(fibers_.Empty());
}

void Mutex::Lock() {
    InjectFault();
    while (owner_) {
        Scheduler::This().Suspend(&fibers_);
    }

    owner_ = Fiber::This();
    InjectFault();
}

bool Mutex::TryLock() {
    InjectFault();
    auto running = Fiber::This();
    if (!owner_) {
        owner_ = running;
    }
    InjectFault();

    return owner_ == running;
}

void Mutex::Unlock() {
    InjectFault();
    owner_ = nullptr;
    fibers_.WakeOne();
    InjectFault();
}

}  // namespace lines

#endif
