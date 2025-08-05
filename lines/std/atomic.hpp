#pragma once

#include <atomic>

#ifdef LINES_THREADS

namespace lines {

template <class T>
using Atomic = std::atomic<T>;

}  // namespace lines

#else

#include <lines/fault/injection.hpp>
#include <lines/util/random.hpp>

namespace lines {

template <class T>
class Atomic : private std::atomic<T> {
public:
    using Impl = std::atomic<T>;
    using Impl::atomic;

    operator T() const noexcept {
        return load();
    }

    T operator=(T value) {
        store(value);
        return value;
    }

    T operator++(int) noexcept {
        return fetch_add(1);
    }

    T operator--(int) noexcept {
        return fetch_sub(1);
    }

    T operator++() noexcept {
        return fetch_add(1) + 1;
    }

    T operator--() noexcept {
        return fetch_sub(1) - 1;
    }

    T operator+=(T value) noexcept {
        return fetch_add(value) + value;
    }

    T operator-=(T value) noexcept {
        return fetch_sub(value) - value;
    }

    // NOLINTNEXTLINE
    T load(std::memory_order mo = std::memory_order::seq_cst) const noexcept {
        InjectFault();
        T value = Impl::load(mo);
        InjectFault();
        return value;
    }

    // NOLINTNEXTLINE
    void store(T value, std::memory_order mo = std::memory_order::seq_cst) noexcept {
        InjectFault();
        Impl::store(value, mo);
        InjectFault();
    }

    // NOLINTNEXTLINE
    T fetch_add(T value, std::memory_order mo = std::memory_order::seq_cst) noexcept {
        InjectFault();
        T result = Impl::fetch_add(value);
        InjectFault();
        return result;
    }

    // NOLINTNEXTLINE
    T fetch_sub(T value, std::memory_order mo = std::memory_order::seq_cst) noexcept {
        InjectFault();
        T result = Impl::fetch_sub(value);
        InjectFault();
        return result;
    }

    // NOLINTNEXTLINE
    T exchange(T new_value, std::memory_order mo = std::memory_order::seq_cst) {
        InjectFault();
        T prev_value = Impl::exchange(new_value, mo);
        InjectFault();
        return prev_value;
    }

    // NOLINTNEXTLINE
    bool compare_exchange_weak(T& expected, T desired, std::memory_order success,
                               std::memory_order failure) {
        InjectFault();

        if (Random(1)) {
            expected = Impl::load(failure);
            return false;
        }

        bool succeeded = Impl::compare_exchange_weak(expected, desired, success, failure);
        InjectFault();
        return succeeded;
    }

    // NOLINTNEXTLINE
    bool compare_exchange_weak(T& expected, T desired,
                               std::memory_order mo = std::memory_order::seq_cst) {
        return compare_exchange_weak(expected, desired, mo, mo);
    }

    // NOLINTNEXTLINE
    bool compare_exchange_strong(T& expected, T desired, std::memory_order success,
                                 std::memory_order failure) {
        InjectFault();
        bool succeeded = Impl::compare_exchange_strong(expected, desired, success, failure);
        InjectFault();
        return succeeded;
    }

    // NOLINTNEXTLINE
    bool compare_exchange_strong(T& expected, T desired,
                                 std::memory_order mo = std::memory_order::seq_cst) noexcept {
        return compare_exchange_strong(expected, desired, mo, mo);
    }
};

}  // namespace lines

#endif
