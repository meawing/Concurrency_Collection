#include <lines/time/api.hpp>

#ifdef LINES_THREADS

#include <thread>

namespace lines {

void SleepFor(const Duration& duration) {
    std::this_thread::sleep_for(duration);
}

}  // namespace lines

#else

#include <lines/fibers/scheduler.hpp>

namespace lines {

void SleepFor(const Duration& duration) {
    Timer timer(Now() + duration);
    Scheduler::This().Sleep(&timer);
}

}  // namespace lines

#endif

namespace lines {

Timepoint Now() {
    return std::chrono::steady_clock::now();
}

}  // namespace lines
