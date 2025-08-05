#include <lines/fibers/api.hpp>
#include <lines/fibers/scheduler.hpp>
#include <lines/util/thread_local.hpp>

namespace lines {

namespace detail {

void Run() {
#ifndef LINES_THREADS
    auto& scheduler = Scheduler::This();
    scheduler.Run();
#endif
}

}  // namespace detail

void Yield() {
#ifndef LINES_THREADS
    auto& scheduler = Scheduler::This();
    scheduler.Yield();
#else
    std::this_thread::yield();
#endif
}

}  // namespace lines
