#include <lines/fibers/handle.hpp>
#include <lines/fibers/scheduler.hpp>

#include <libassert/assert.hpp>

namespace lines {

#ifndef LINES_THREADS

Handle::Handle(Handle&& other) {
    std::swap(fiber_, other.fiber_);
    if (fiber_) {
        fiber_->handle_ = this;
    }
}

Handle& Handle::operator=(Handle&& other) {
    std::swap(fiber_, other.fiber_);
    if (fiber_) {
        fiber_->handle_ = this;
    }
    if (other.fiber_) {
        other.fiber_->handle_ = &other;
    }
    return *this;
}

Handle::~Handle() {
    ASSERT(!joinable());
}

void Handle::Schedule() {
    auto& scheduler = Scheduler::This();
    scheduler.Schedule(fiber_);
}

void Handle::detach() {
    if (fiber_) {
        fiber_->handle_ = nullptr;
        fiber_ = nullptr;
    }
}

void Handle::join() {
    if (fiber_) {
        auto& scheduler = Scheduler::This();
        scheduler.Suspend(fiber_);
    }
}

bool Handle::joinable() {
    return fiber_;
}

#endif

}  // namespace lines
