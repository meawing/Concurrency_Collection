#pragma once

namespace lines {

class Fiber;

class IAwaitable {
public:
    virtual void Park(Fiber* fiber) = 0;
};

}  // namespace lines
