#pragma once

#include <utility>

namespace lines {

template <class F>
class Defer {
public:
    Defer(F&& f) : f_(std::move(f)) {
    }

    ~Defer() {
        f_();
    }

private:
    F f_;
};

}  // namespace lines
