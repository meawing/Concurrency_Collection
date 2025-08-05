#include <lines/fibers/scheduler.hpp>
#include <lines/util/logger.hpp>
#include <lines/util/random.hpp>
#include <lines/sync/awaitable.hpp>
#include <lines/time/api.hpp>
#include <lines/time/timer.hpp>

#include <libassert/assert.hpp>

namespace lines {

static thread_local Scheduler scheduler;

void Scheduler::Run() {
    auto& logger = DefaultLogger();

    while (Step()) {
    }

    ASSERT(fibers_.Empty(), "Deadlock detected");
    ASSERT(running_ == nullptr);
}

void Scheduler::Schedule(Fiber* fiber) {
    if (fiber->GetState() == Fiber::State::Dead) {
        ASSERT(fiber == running_);
        SwitchToSched();
    } else {
        ASSERT(fiber->GetState() == Fiber::State::Runnable);
        fibers_.Prepend(fiber);
    }
}

void Scheduler::Suspend(IAwaitable* awaitable) {
    running_->SetState(Fiber::State::Suspended);
    awaitable->Park(running_);

    SwitchToSched();

    ASSERT(running_->GetState() == Fiber::State::Running);
}

void Scheduler::Sleep(Timer* timer) {
    timers_.Add(timer);
    Suspend(timer);
}

void Scheduler::Yield() {
    ASSERT(running_->GetState() == Fiber::State::Running);
    running_->SetState(Fiber::State::Runnable);
    SwitchToSched();
}

Scheduler& Scheduler::This() {
    return scheduler;
}

Fiber* Scheduler::Running() {
    return This().running_;
}

bool Scheduler::Step() {
    bool fibers = FiberStep();
    bool timers = TimerPoll();

    return fibers || timers;
}

bool Scheduler::FiberStep() {
    auto fiber = fibers_.PickRandom(/*runnable=*/true);
    if (!fiber) {
        return false;
    }

    fibers_.Remove(fiber);

    running_ = fiber;
    ASSERT(running_->GetState() == Fiber::State::Runnable);
    running_->SetState(Fiber::State::Running);
    SwitchToFiber(running_);
    running_ = nullptr;

    if (fiber->GetState() == Fiber::State::Dead) {
        delete fiber;
    } else if (fiber->GetState() == Fiber::State::Runnable) {
        fibers_.Prepend(fiber);
    } else {
        ASSERT(fiber->GetState() == Fiber::State::Suspended);
    }

    return true;
}

bool Scheduler::TimerPoll() {
    if (timers_.Empty()) {
        return false;
    }

    Timepoint tp = Now();
    while (!timers_.Empty() && timers_.Top()->CompareWithTimepoint(tp)) {
        auto fiber = timers_.Top()->Unpark();
        timers_.Pop();
        ASSERT(fiber->GetState() == Fiber::State::Suspended);
        fiber->SetState(Fiber::State::Runnable);
        Schedule(fiber);
    }

    return true;
}

void Scheduler::SwitchToFiber(Fiber* fiber) {
    sched_ctx_.Switch(fiber->GetContext());
}

void Scheduler::SwitchToSched() {
    if (running_->GetState() == Fiber::State::Dead) {
        running_->GetContext().SwitchLast(sched_ctx_);
    } else {
        running_->GetContext().Switch(sched_ctx_);
    }
}

}  // namespace lines
