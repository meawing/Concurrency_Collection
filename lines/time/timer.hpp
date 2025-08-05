#pragma once

#include <lines/sync/awaitable.hpp>
#include <lines/time/api.hpp>

namespace lines {

class Timer : public IAwaitable {
public:
    Timer(const Timepoint& timepoint) : timepoint_(std::move(timepoint)) {
    }
    Timer(const Timer&) = delete;
    Timer(Timer&&) = default;

    void Park(Fiber* fiber) final {
        fiber_ = fiber;
    }
    Fiber* Unpark() {
        auto fiber = fiber_;
        fiber_ = nullptr;
        return fiber;
    }

    bool operator<(const Timer& timer) {
        return timepoint_ < timer.timepoint_;
    }

    bool CompareWithTimepoint(const Timepoint& timepoint) {
        return timepoint_ <= timepoint;
    }

private:
    Timepoint timepoint_;
    Fiber* fiber_ = nullptr;
};

}  // namespace lines
