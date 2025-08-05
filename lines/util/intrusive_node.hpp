#pragma once

namespace lines {

template <class Base>
struct IntrusiveNode {
    Base* Obj() {
        return static_cast<Base*>(this);
    }

    void Unlink() {
        if (next) {
            next->prev = prev;
        }
        if (prev) {
            prev->next = next;
        }

        next = nullptr;
        prev = nullptr;
    }

    Base* Next() {
        return next;
    }

    Base* next = nullptr;
    Base* prev = nullptr;
};

}  // namespace lines
