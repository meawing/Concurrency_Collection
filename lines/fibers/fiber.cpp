#include <lines/fibers/fiber.hpp>
#include <lines/fibers/scheduler.hpp>
#include <lines/fibers/handle.hpp>

#include <libassert/assert.hpp>

namespace lines {

constexpr size_t kStorageSize = 1 << 12;

Fiber::~Fiber() {
    if (waiter_) {
        waiter_->state_ = Fiber::State::Runnable;
        Scheduler::This().Schedule(waiter_);
    }
}

void Fiber::Run() {
    state_ = State::Running;

    try {
        // Allocate thread local storage on fiber's stack.
        alignas(16) std::array<std::byte, kStorageSize> tls{};
        tls_view_ = tls;
        routine_();
    } catch (...) {
    }

    state_ = State::Dead;

    if (handle_) {
        handle_->detach();
    }

    Scheduler::This().Schedule(this);

    UNREACHABLE();
}

void Fiber::Park(Fiber* fiber) {
    ASSERT(!waiter_);
    waiter_ = fiber;
}

Fiber* Fiber::This() {
    return Scheduler::This().Running();
}

Context& Fiber::GetContext() {
    return ctx_;
}

std::span<std::byte> Fiber::GetTLSView() {
    return tls_view_;
}

Fiber::State Fiber::GetState() {
    return state_;
}

void Fiber::SetState(State state) {
    state_ = state;
}

}  // namespace lines
