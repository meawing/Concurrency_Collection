#pragma once

#include <ctime>
#include <optional>

#include <lines/time/api.hpp>

namespace lines {

class CpuClock {
public:
    void Start();
    double Finish();

private:
    std::optional<std::clock_t> start_;
};

class WallClock {
public:
    void Start();
    double Finish();

private:
    std::optional<Timepoint> start_;
};

bool IsClockLess(double cpu, double wall);

}  // namespace lines
