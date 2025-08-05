#pragma once

#include <lines/fibers/fiber.hpp>
#include <lines/fibers/queue.hpp>
#include <lines/time/queue.hpp>
#include <lines/time/timer.hpp>
#include <lines/sync/awaitable.hpp>

namespace lines {

class Scheduler {
public:
    void Run();

    void Schedule(Fiber* fiber);
    void Suspend(IAwaitable* awaitable);
    void Sleep(Timer* awaitable);
    void Yield();

    static Scheduler& This();
    static Fiber* Running();

private:
    bool Step();
    bool FiberStep();
    bool TimerPoll();

    void SwitchToFiber(Fiber* fiber);
    void SwitchToSched();

private:
    FiberQueue fibers_;
    TimerQueue timers_;

    Context sched_ctx_;
    Fiber* running_ = nullptr;
};

}  // namespace lines
