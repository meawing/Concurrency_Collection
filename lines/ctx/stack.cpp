#include <lines/ctx/stack.hpp>

#include <libassert/assert.hpp>
#include <unistd.h>

#include <sys/mman.h>

namespace lines {

namespace {

size_t PageSize() {
    auto result = sysconf(_SC_PAGESIZE);
    ASSERT(result >= 0);
    return static_cast<size_t>(result);
}

const size_t kPageSize = PageSize();                        // Usually 4 KiB.
const size_t kStackSize = (1ul << 11) * kPageSize;          // Usually 8 MiB.
const size_t kAllocationSize = 2 * kPageSize + kStackSize;  // Protect stack from both sides.

void ProtectPage(void* address) {
    int ret = mprotect(address, kPageSize, PROT_NONE);
    ASSERT(ret == 0);
}

}  // namespace

Stack::Stack() {
    allocation_ =
        mmap(nullptr, kAllocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT(allocation_ != nullptr);
    ProtectPage(allocation_);
    ProtectPage(static_cast<std::byte*>(allocation_) + kPageSize + kStackSize);
}

Stack::~Stack() {
    if (allocation_) {
        int ret = munmap(allocation_, kAllocationSize);
        ASSERT(ret == 0);
    }
}

Stack::Stack(Stack&& other) {
    std::swap(other.allocation_, allocation_);
}

Stack& Stack::operator=(Stack&& other) {
    std::swap(allocation_, other.allocation_);
    return *this;
}

std::span<std::byte> Stack::GetStackView() {
    return {static_cast<std::byte*>(allocation_) + kPageSize, kStackSize};
}

}  // namespace lines
