#pragma once

#include <span>

namespace lines {

class Stack {
public:
    Stack();
    ~Stack();

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    Stack(Stack&& other);
    Stack& operator=(Stack&&);

    std::span<std::byte> GetStackView();

private:
    void* allocation_{};
};

}  // namespace lines
