#pragma once

namespace lines {

template <typename T>
__attribute__((noinline)) void DoNotOptimize(T&& value) {
    asm volatile("" : : "X"(value));
}

}  // namespace lines
