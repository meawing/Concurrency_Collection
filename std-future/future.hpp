// future.hpp
#pragma once

#include <utility>

#include "promise.hpp"
#include "state.hpp"

namespace future {

template <class T>
class Future {
public:
    Future(Promise<T>& promise) : state_(promise.state_) {
        // Ensure we have a valid state
        if (!state_) {
            throw std::runtime_error("Invalid state in Promise");
        }
    }

    Future(const Future&) = delete;
    Future(Future&& other) noexcept : state_(std::exchange(other.state_, nullptr)) {
    }

    ~Future() = default;

    T Get() && {
        if (!state_) {
            throw std::runtime_error("Future has no state");
        }
        // Create a local copy of the shared_ptr to ensure it doesn't get destroyed
        // during the Get call even if the Future is destroyed
        auto state_copy = state_;
        return state_copy->Get();
    }

private:
    std::shared_ptr<detail::SharedState<T>> state_;
};

template <class T>
std::pair<Future<T>, Promise<T>> GetTied() {
    Promise<T> p;
    Future<T> f(p);

    return std::make_pair(std::move(f), std::move(p));
}

}  // namespace future
