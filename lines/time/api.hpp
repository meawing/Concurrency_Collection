#pragma once

#include <chrono>

using namespace std::literals::chrono_literals;

namespace lines {

using Duration = std::chrono::milliseconds;
using Timepoint = std::chrono::time_point<std::chrono::steady_clock>;

void SleepFor(const Duration& duration);

Timepoint Now();

}  // namespace lines
