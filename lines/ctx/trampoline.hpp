#pragma once

namespace lines {

class ITrampoline {
public:
    virtual void Run() = 0;
    virtual ~ITrampoline() = default;
};

}  // namespace lines
