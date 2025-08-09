#pragma once

#include <functional>

#include <lines/ctx/ctx.hpp>
#include <lines/ctx/trampoline.hpp>
#include <lines/ctx/stack.hpp>

using Routine = std::function<void()>;

class Coroutine : public lines::ITrampoline {
public:
    Coroutine(Routine routine);

    // Resume the coroutine from where it left off.
    void Resume();

    // Called from within a coroutine to yield control back to its caller.
    static void Suspend();

    // Returns true if the coroutine’s routine has finished.
    bool IsCompleted();

private:
    // This is called when the coroutine is first scheduled.
    void Run() final;

private:
    Routine routine_;
    bool completed_ = false;
    lines::Stack stack_;
    lines::Context ctx_;         // The coroutine’s own context.
    lines::Context caller_ctx_;  // The saved caller’s context.

    // A thread-local pointer to the currently running coroutine.
    // (This is used by the static Suspend() method to know which coroutine to switch.)
    static inline thread_local Coroutine* current = nullptr;
};
