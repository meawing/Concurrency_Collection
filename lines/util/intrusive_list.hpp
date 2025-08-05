#pragma once

#include <lines/util/intrusive_node.hpp>

#include <cstddef>

namespace lines {

template <class T>
class IntrusiveList {
public:
    void Prepend(T* obj) {
        if (head_) {
            obj->next = head_;
            head_->prev = obj;
        }

        head_ = obj;
        ++size_;
    }

    void Remove(T* obj) {
        if (obj == head_) {
            head_ = obj->next;
        }

        obj->Unlink();
        --size_;
    }

    T* Head() {
        return head_;
    }

    bool Empty() const {
        return head_ == nullptr;
    }

    std::size_t Size() const {
        return size_;
    }

private:
    T* head_ = nullptr;
    size_t size_ = 0;
};

}  // namespace lines
