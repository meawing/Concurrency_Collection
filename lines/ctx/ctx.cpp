#include <lines/ctx/ctx.hpp>

#if __has_feature(address_sanitizer)
#include <sanitizer/common_interface_defs.h>
#elif __has_feature(thread_sanitizer)
#include <sanitizer/tsan_interface.h>
#endif

#include <cstdlib>
#include <libassert/assert.hpp>

namespace lines {

static void StaticTrampoline(void*, void*, void*, void*, void*, void*, void* arg) {
    static_cast<ITrampoline*>(arg)->Run();
}

extern "C" void* SetupContext(void* stack, void* trampoline, void* arg);
extern "C" void SwitchContext(void* from_rsp, void* to_rsp);

void Context::Run() {
#if __has_feature(address_sanitizer)
    __sanitizer_finish_switch_fiber(nullptr, &(from_->stack_bottom_), &(from_->stack_size_));
    from_ = nullptr;
#elif __has_feature(thread_sanitizer)
    DestroyFinishedTsanFiber();
#endif

    user_trampoline_->Run();

    UNREACHABLE();
}

void Context::Setup(std::span<std::byte> stack, ITrampoline* trampoline) {
#if __has_feature(address_sanitizer)
    stack_bottom_ = stack.data() + stack.size();
    stack_size_ = stack.size();
#elif __has_feature(thread_sanitizer)
    ASSERT(tsan_fiber_ == nullptr);
    tsan_fiber_ = __tsan_create_fiber(0);
#endif

    user_trampoline_ = trampoline;
    rsp_ =
        SetupContext(stack.data() + stack.size(), reinterpret_cast<void*>(StaticTrampoline), this);
}

void Context::Switch(Context& to) {
#if __has_feature(address_sanitizer)
    to.from_ = this;
    __sanitizer_start_switch_fiber(&fake_stack_, to.stack_bottom_, to.stack_size_);
#elif __has_feature(thread_sanitizer)
    tsan_fiber_ = __tsan_get_current_fiber();
    __tsan_switch_to_fiber(to.tsan_fiber_, 0);
#endif

    SwitchContext(&rsp_, &to.rsp_);

#if __has_feature(address_sanitizer)
    __sanitizer_finish_switch_fiber(fake_stack_, &(from_->stack_bottom_), &(from_->stack_size_));
    from_ = nullptr;
#elif __has_feature(thread_sanitizer)
    DestroyFinishedTsanFiber();
#endif
}

void Context::SwitchLast(Context& to) {
#if __has_feature(address_sanitizer)
    to.from_ = this;
    __sanitizer_start_switch_fiber(nullptr, to.stack_bottom_, to.stack_size_);
#elif __has_feature(thread_sanitizer)
    to.finished_tsan_fiber_ = __tsan_get_current_fiber();
    __tsan_switch_to_fiber(to.tsan_fiber_, 0);
#endif

    SwitchContext(&rsp_, &to.rsp_);

    UNREACHABLE();
}

#if __has_feature(thread_sanitizer)

void Context::DestroyFinishedTsanFiber() {
    if (finished_tsan_fiber_) {
        __tsan_destroy_fiber(finished_tsan_fiber_);
        finished_tsan_fiber_ = nullptr;
    }
}

#endif

}  // namespace lines
