#pragma once

#include <atomic>

namespace luisa::compute::fallback {

class FallbackEvent {

private:
    std::atomic_uint64_t _fence_value{0u};

public:
    void signal(uint64_t fence_value) noexcept;
    void wait(uint64_t fence_value) const noexcept;
    [[nodiscard]] auto is_completed(uint64_t fence_value) const noexcept { return _fence_value.load(std::memory_order_acquire) >= fence_value; }
    [[nodiscard]] auto native_handle() noexcept { return &_fence_value; }
    [[nodiscard]] auto native_handle() const noexcept { return &_fence_value; }
};

}// namespace luisa::compute::fallback
