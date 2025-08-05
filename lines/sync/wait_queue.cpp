#include <lines/sync/wait_queue.hpp>
#include <lines/fibers/fiber.hpp>
#include <lines/util/random.hpp>
#include <lines/fibers/scheduler.hpp>

#include <libassert/assert.hpp>

namespace lines {

void WaitQueue::Park(Fiber* fiber) {
    ASSERT(fiber->GetState() == Fiber::State::Suspended);
    fibers_.Prepend(fiber);
}

void WaitQueue::WakeOne() {
    auto fiber = fibers_.PickRandom();
    if (!fiber) {
        return;
    }
    ASSERT(fiber->GetState() == Fiber::State::Suspended);

    fibers_.Remove(fiber);
    fiber->SetState(Fiber::State::Runnable);
    Scheduler::This().Schedule(fiber);
}

void WaitQueue::WakeAll() {
    Fiber* next = nullptr;
    for (Fiber* fiber = fibers_.Head(); fiber; fiber = next) {
        ASSERT(fiber->GetState() == Fiber::State::Suspended);

        next = fiber->Next();

        fibers_.Remove(fiber);
        fiber->SetState(Fiber::State::Runnable);
        Scheduler::This().Schedule(fiber);
    }
}

WaitQueue::~WaitQueue() {
    ASSERT(fibers_.Empty());
}

}  // namespace lines
