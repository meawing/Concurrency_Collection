#pragma once

#include <libassert/assert.hpp>
#include <memory>
#include <atomic>

#include "coro/coroutine.hpp"
#include "tp/tp.hpp"
#include <lines/util/thread_local.hpp>

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    enum class State { Runnable, Running, Suspended, Dead };

    template <class F>
    explicit Fiber(F&& f, ThreadPool* tp = nullptr)
        : routine_(std::forward<F>(f)), tp_(tp), state_(State::Runnable) {
        coro_ = new Coroutine([this]() {
            try {
                routine_();
            } catch (...) {
                // Handle exceptions
            }
            state_ = State::Dead;
        });
    }

    ~Fiber() {
        delete coro_;
    }

    void Resume();
    void Suspend();

    State GetState() const {
        return state_;
    }

    static Fiber* This();

private:
    void Schedule();

private:
    Routine routine_;
    Coroutine* coro_;
    ThreadPool* tp_;
    std::atomic<State> state_;

    static thread_local Fiber* current_fiber;
};

namespace api {

template <class F>
void Spawn(F&& f, ThreadPool* tp) {
    // Use shared_ptr to manage the fiber's lifetime
    auto fiber = std::make_shared<Fiber>(std::forward<F>(f), tp);

    // Capture the shared_ptr in the lambda to keep the fiber alive
    if (tp) {
        tp->Submit([fiber]() { fiber->Resume(); });
    } else {
        fiber->Resume();
    }
}

template <class F>
void Spawn(F&& f) {
    Spawn(std::forward<F>(f), ThreadPool::This());
}

void Yield();

}  // namespace api
