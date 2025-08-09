#include "coroutine.hpp"

#include <libassert/assert.hpp>
#include <utility>

Coroutine::Coroutine(Routine routine) : routine_(std::move(routine)), completed_(false) {
    // Allocate a stack and setup our context.
    // We pass "this" as the trampoline so that when ctx_ is entered, Run() is called.
    ctx_.Setup(stack_.GetStackView(), this);
}

void Coroutine::Run() {
    // When a coroutine first starts, ensure that the current pointer is ours.
    current = this;
    // Execute the user-supplied routine.
    routine_();
    // Mark our coroutine as finished.
    completed_ = true;
    // When finished, permanently switch back to our caller.
    ctx_.SwitchLast(caller_ctx_);
    UNREACHABLE();
}

void Coroutine::Resume() {
    if (completed_) {
        return;
    }
    // Save any previously running coroutine (this is useful if we have nested coroutines).
    Coroutine* previous = current;
    // Set ourselves as currently running.
    current = this;
    // Save the caller’s execution state in our own caller_ctx_ and switch to our context.
    caller_ctx_.Switch(ctx_);
    // When we resume here (after our coroutine yielded or ended),
    // restore the previous running coroutine (which might be nullptr).
    current = previous;
}

void Coroutine::Suspend() {
    // This must only be called from inside a running coroutine.
    ASSERT(current != nullptr);
    // Switch from the currently running coroutine back to the caller.
    current->ctx_.Switch(current->caller_ctx_);
}

bool Coroutine::IsCompleted() {
    return completed_;
}
