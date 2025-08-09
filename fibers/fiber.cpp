#include "fiber.hpp"

#include <lines/util/thread_local.hpp>

thread_local Fiber* Fiber::current_fiber = nullptr;

void Fiber::Resume() {
    if (state_ == State::Dead) {
        return;
    }

    Fiber* prev = current_fiber;
    current_fiber = this;

    state_ = State::Running;
    coro_->Resume();

    // After resuming, check the state
    if (state_ == State::Dead) {
        // The fiber has completed its execution
    } else if (state_ == State::Suspended) {
        // The fiber has yielded, reschedule it
        if (tp_) {
            Schedule();
        }
    }

    current_fiber = prev;
}

void Fiber::Suspend() {
    ASSERT(current_fiber == this);
    state_ = State::Suspended;
    coro_->Suspend();
    // When we return here, we've been resumed
    state_ = State::Running;
}

void Fiber::Schedule() {
    if (tp_) {
        // Create a shared_ptr from this to keep the fiber alive during scheduling
        auto self = shared_from_this();
        tp_->Submit([self]() { self->Resume(); });
    }
}

Fiber* Fiber::This() {
    return current_fiber;
}

namespace api {

void Yield() {
    Fiber* current = Fiber::This();
    if (current) {
        current->Suspend();
    }
}

}  // namespace api
