#pragma once

#include <lines/ctx/trampoline.hpp>

#include <cstddef>
#include <span>

namespace lines {

class Context : public ITrampoline {
public:
    void Setup(std::span<std::byte> stack, ITrampoline* trampoline);

    void Switch(Context& to);
    [[noreturn]] void SwitchLast(Context& to);

private:
    [[noreturn]] void Run() final;

private:
    void* rsp_{};
    ITrampoline* user_trampoline_{};

#if __has_feature(address_sanitizer)
    Context* from_{};

    const void* stack_bottom_{};
    size_t stack_size_{};

    void* fake_stack_{};
#elif __has_feature(thread_sanitizer)

    void DestroyFinishedTsanFiber();

    void* tsan_fiber_{};
    void* finished_tsan_fiber_{};
#endif
};

}  // namespace lines
