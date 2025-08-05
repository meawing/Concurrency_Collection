#include <lines/std/condvar.hpp>
#include <lines/fault/injection.hpp>

#include <libassert/assert.hpp>

#ifdef LINES_THREADS

namespace lines {

void Condvar::NotifyOne() {
    InjectFault();
    condvar_.notify_one();
    InjectFault();
}

void Condvar::NotifyAll() {
    InjectFault();
    condvar_.notify_all();
    InjectFault();
}

void Condvar::Injection() {
    InjectFault();
}

}  // namespace lines

#else

#include <lines/fibers/scheduler.hpp>

namespace lines {

Condvar::~Condvar() {
    ASSERT(fibers_.Empty());
}

void Condvar::NotifyOne() {
    InjectFault();
    fibers_.WakeOne();
    InjectFault();
}

void Condvar::NotifyAll() {
    InjectFault();
    fibers_.WakeAll();
    InjectFault();
}

void Condvar::StartWait() {
    InjectFault();

    DisableInjection();
}

void Condvar::Suspend() {
    Scheduler::This().Suspend(&fibers_);
}

void Condvar::EndWait() {
    EnableInjection();

    InjectFault();
}

}  // namespace lines

#endif
