#pragma once

#include <queue>

#include <lines/fibers/fiber.hpp>
#include <lines/time/timer.hpp>

namespace lines {

class TimerQueue {
public:
    void Add(Timer* timer) {
        timers_.emplace(timer);
    }

    bool Empty() {
        return timers_.empty();
    }

    Timer* Top() const {
        return timers_.top();
    }

    void Pop() {
        timers_.pop();
    }

private:
    struct Comparator {
        bool operator()(Timer* lhs, Timer* rhs) {
            return *lhs < *rhs;
        }
    };

private:
    std::priority_queue<Timer*, std::vector<Timer*>, Comparator> timers_;
};

}  // namespace lines
