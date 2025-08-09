// promise.hpp
#pragma once

#include <memory>

#include "state.hpp"

namespace future {

template <class T>
class Future;

template <class T>
class Promise {
public:
    Promise() : state_(std::make_shared<detail::SharedState<T>>()) {
    }
    Promise(const Promise&) = delete;
    Promise(Promise&& other) noexcept : state_(std::exchange(other.state_, nullptr)) {
    }

    ~Promise() = default;

    void SetValue(T value) && {
        if (!state_) {
            throw std::runtime_error("Promise has no state");
        }
        // Create a local copy of the shared_ptr to ensure it doesn't get destroyed
        // during the SetValue call even if the Promise is moved from or destroyed
        auto state_copy = state_;
        state_copy->SetValue(std::move(value));
    }

    void SetError(Error error) && {
        if (!state_) {
            throw std::runtime_error("Promise has no state");
        }
        // Create a local copy of the shared_ptr to ensure it doesn't get destroyed
        // during the SetError call even if the Promise is moved from or destroyed
        auto state_copy = state_;
        state_copy->SetError(std::move(error));
    }

private:
    friend class Future<T>;
    std::shared_ptr<detail::SharedState<T>> state_;
};

}  // namespace future
