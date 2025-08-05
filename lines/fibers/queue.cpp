#include <lines/fibers/queue.hpp>
#include <lines/fibers/fiber.hpp>
#include <lines/util/random.hpp>
#include <lines/fibers/scheduler.hpp>

namespace lines {

Fiber* FiberQueue::PickRandom(bool runnable) {
    int num_hops = Random(Size());

    Fiber* victim = Pick(Head(), runnable);
    if (!victim) {
        return nullptr;
    }

    for (int i = 0; i < num_hops; ++i) {
        Fiber* next = PickNext(victim, runnable);
        if (!next) {
            next = Pick(Head(), runnable);
        }

        victim = next;
    }

    return victim;
}

Fiber* FiberQueue::Pick(Fiber* start, bool runnable) {
    Fiber* fiber = start;
    while (fiber && fiber->GetState() != Fiber::State::Runnable && runnable) {
        fiber = fiber->Next();
    }

    return fiber;
}

Fiber* FiberQueue::PickNext(Fiber* fiber, bool runnable) {
    fiber = fiber->Next();
    return Pick(fiber, runnable);
}

}  // namespace lines
