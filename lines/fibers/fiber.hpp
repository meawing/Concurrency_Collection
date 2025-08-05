#pragma once

#include <lines/util/intrusive_node.hpp>
#include <lines/ctx/ctx.hpp>
#include <lines/ctx/stack.hpp>
#include <lines/ctx/trampoline.hpp>
#include <lines/sync/awaitable.hpp>

#include <function2/function2.hpp>

#include <thread>

#ifdef LINES_THREADS

namespace lines {

using Handle = std::thread;

}  // namespace lines

#endif

namespace lines {

using Routine = fu2::unique_function<void()>;

#ifndef LINES_THREADS
class Handle;
#endif

class Fiber : public IntrusiveNode<Fiber>, public ITrampoline, public IAwaitable {
public:
    enum class State {
        Runnable,
        Running,
        Dead,
        Suspended,
    };

public:
    template <class F>
    explicit Fiber(F&& f, Handle* handle) : routine_(std::forward<F>(f)), handle_(handle) {
        ctx_.Setup(stack_.GetStackView(), this);
    }

    ~Fiber() override;

    void Run() final;
    void Park(Fiber* fiber) final;

    Context& GetContext();
    std::span<std::byte> GetTLSView();

    State GetState();
    void SetState(State state);

    static Fiber* This();

private:
    Routine routine_;
    Stack stack_;
    Context ctx_;

    Handle* handle_{};
    Fiber* waiter_{};
    State state_ = State::Runnable;

    std::span<std::byte> tls_view_{};

#ifndef LINES_THREADS
private:
    friend class Handle;
#endif
};

}  // namespace lines
