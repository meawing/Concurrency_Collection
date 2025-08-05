#pragma once

#include <lines/util/intrusive_list.hpp>

namespace lines {

class Fiber;

class FiberQueue : public IntrusiveList<Fiber> {
public:
    Fiber* PickRandom(bool runnable = false);

    static Fiber* Pick(Fiber* start, bool runnable);
    static Fiber* PickNext(Fiber* fiber, bool runnable);
};

}  // namespace lines
