// state.hpp
#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

#include <lines/std/mutex.hpp>
#include <lines/std/condvar.hpp>

namespace future {

using Error = std::exception_ptr;

namespace detail {

template <class T>
class SharedState {
public:
    SharedState() = default;

    void SetValue(T value) {
        std::lock_guard<lines::Mutex> guard(mutex_);
        if (!result_.has_value()) {
            result_.emplace(std::in_place_index<0>, std::move(value));
            condvar_.NotifyAll();
        }
    }

    void SetError(Error error) {
        std::lock_guard<lines::Mutex> guard(mutex_);
        if (!result_.has_value()) {
            result_.emplace(std::in_place_index<1>, std::move(error));
            condvar_.NotifyAll();
        }
    }

    T Get() {
        std::unique_lock<lines::Mutex> lock(mutex_);

        // Wait until a value or error is set
        while (!result_.has_value()) {
            condvar_.Wait(lock);
        }

        // Check if we have a value or an error
        if (result_->index() == 0) {
            return std::move(std::get<0>(*result_));
        } else {
            std::rethrow_exception(std::get<1>(*result_));
        }
    }

private:
    lines::Mutex mutex_;
    lines::Condvar condvar_;
    std::optional<std::variant<T, Error>> result_;
};

}  // namespace detail

}  // namespace future
