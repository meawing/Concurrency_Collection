#pragma once

#include <libassert/assert.hpp>

#include <lines/std/thread.hpp>

#ifdef LINES_THREADS

namespace lines {

template <class T>
using ThreadLocalPtr = T*;

}  // namespace lines

#else

#include <lines/ctx/stack.hpp>
#include <lines/fibers/fiber.hpp>
#include <lines/util/logger.hpp>

namespace lines {

template <class T>
class ThreadLocalPtr {
public:
    ThreadLocalPtr(T* ptr) : shift_(AllocateShift()) {
        Create(shift_, ptr);
    }

    ThreadLocalPtr(const ThreadLocalPtr<T>&) = delete;
    ThreadLocalPtr(ThreadLocalPtr<T>&&) = delete;

    ThreadLocalPtr<T>& operator=(const ThreadLocalPtr<T>&) = delete;
    ThreadLocalPtr<T>& operator=(ThreadLocalPtr<T>&&) = delete;

    T* operator=(T* ptr) {
        void*& stored = Access(shift_);
        stored = ptr;
        return reinterpret_cast<T*>(stored);
    }

    T& operator*() {
        return *reinterpret_cast<T*>(Access(shift_));
    }

    T* operator->() {
        return reinterpret_cast<T*>(Access(shift_));
    }

    operator T*() {
        return reinterpret_cast<T*>(Access(shift_));
    }

    operator bool() const {
        return reinterpret_cast<T*>(Access(shift_)) != nullptr;
    }

private:
    static void Create(size_t shift, void* ptr) {
        auto& stored = Access(shift);
        stored = ptr;
    }

    static void*& Access(size_t shift) {
        ASSERT(Fiber::This());
        auto tls_view = Fiber::This()->GetTLSView();
        void** ptr =
            reinterpret_cast<void**>(tls_view.data() + sizeof(size_t) + shift * sizeof(void*));
        return *ptr;
    }

    static size_t AllocateShift() {
        ASSERT(Fiber::This());
        auto tls_view = Fiber::This()->GetTLSView();

        size_t& tls_size = *reinterpret_cast<size_t*>(tls_view.data());

        ASSERT((tls_size + 1) * sizeof(void*) + sizeof(size_t) <= tls_view.size(),
               "Fiber local storage overflow");

        return tls_size++;
    }

private:
    size_t shift_;
};

}  // namespace lines

#endif
