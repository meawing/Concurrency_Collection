#pragma once

#include <thread>

#ifdef LINES_THREADS

namespace lines {

using Handle = std::thread;

}  // namespace lines

#else

#include <lines/fibers/fiber.hpp>

namespace lines {

class Handle {
public:
    Handle() = default;

    template <class F>
    explicit Handle(F&& f) : fiber_(new Fiber(std::forward<F>(f), this)) {
        Schedule();
    }

    Handle(const Handle&) = delete;
    const Handle& operator=(const Handle&) = delete;

    Handle(Handle&& other);
    Handle& operator=(Handle&& other);

    void detach();    // NOLINT
    void join();      // NOLINT
    bool joinable();  // NOLINT

    ~Handle();

private:
    void Schedule();

private:
    Fiber* fiber_{};
};

}  // namespace lines

#endif
