#include <lines/util/clock.hpp>

namespace lines {

void CpuClock::Start() {
    start_ = std::clock();
}

double CpuClock::Finish() {
    return 1000.0 * (std::clock() - *start_) / CLOCKS_PER_SEC;
}

void WallClock::Start() {
    start_ = std::chrono::steady_clock::now();
}

double WallClock::Finish() {
    return std::chrono::duration_cast<Duration>(std::chrono::steady_clock::now() - *start_).count();
}

bool IsClockLess(double cpu, double wall) {
#ifdef LINES_THREADS
    return cpu < wall;
#else
    return true;
#endif
}

}  // namespace lines
