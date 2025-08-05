#pragma once

#include <lines/fibers/handle.hpp>

#include <utility>

namespace lines {

namespace detail {

void Run();

}  // namespace detail

template <class F>
auto Spawn(F&& f) {
    return Handle(std::forward<F>(f));
}

void Yield();

template <class F>
void SchedulerRun(F&& f, size_t num_runs = 10) {
    for (size_t run = 0; run < num_runs; ++run) {
        auto handle = Spawn(f);

#ifndef LINES_THREADS
        detail::Run();
#else
        handle.join();
#endif
    }
}

}  // namespace lines
