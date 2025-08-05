#include <lines/fault/injection.hpp>
#include <lines/fibers/api.hpp>

#include <atomic>
#include <libassert/assert.hpp>

namespace lines {

class Injector {
public:
    Injector(size_t period) : period_(period), counter_(0), enabled_(true) {
        ASSERT((period_ & (period_ - 1)) == 0);
    }

    void Fault() {
        if ((counter_.fetch_add(1) & (period_ - 1)) == 0 && enabled_.load() && InFiber()) {
            Yield();
        }
    }

    void Disable() {
        enabled_.store(false);
    }

    void Enable() {
        enabled_.store(true);
    }

private:
    bool InFiber() {
#ifdef LINES_THREADS
        return true;
#else
        return Fiber::This();
#endif
    }

private:
    size_t period_;
    std::atomic<size_t> counter_;
    std::atomic<bool> enabled_;
};

#ifdef LINES_THREADS
static const size_t kPeriod = 32;
#else
static const size_t kPeriod = 4;
#endif

static Injector injector(kPeriod);

void InjectFault() {
    injector.Fault();
}

void DisableInjection() {
    injector.Disable();
}

void EnableInjection() {
    injector.Enable();
}

}  // namespace lines
