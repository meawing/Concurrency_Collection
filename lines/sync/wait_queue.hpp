#pragma once

#include <lines/fibers/queue.hpp>
#include <lines/sync/awaitable.hpp>

namespace lines {

class WaitQueue : public IAwaitable {
public:
    void Park(Fiber* fiber) override;
    void WakeOne();
    void WakeAll();

    bool Empty() {
        return fibers_.Empty();
    }

    ~WaitQueue();

private:
    FiberQueue fibers_;
};

}  // namespace lines
