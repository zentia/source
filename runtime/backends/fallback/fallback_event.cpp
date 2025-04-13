#include <thread>
#include <chrono>

#include "fallback_event.h"

namespace luisa::compute::fallback {

void FallbackEvent::signal(uint64_t fence_value) noexcept {
#if defined(__has_builtin) && __has_builtin(__c11_atomic_fetch_max)
    using A = _Atomic(uint64_t);
    auto p = reinterpret_cast<A *>(&_fence_value);
    __c11_atomic_fetch_max(p, fence_value, __ATOMIC_RELEASE);
#else
    // use a CAS loop to ensure the fence value is monotonically increasing
    auto current = _fence_value.load(std::memory_order_acquire);
    while (current < fence_value) {
        if (_fence_value.compare_exchange_weak(current, fence_value, std::memory_order_release, std::memory_order_acquire)) {
            break;
        }
    }
#endif
}

void FallbackEvent::wait(uint64_t fence_value) const noexcept {
    // spin until the fence value is reached
    while (!is_completed(fence_value)) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50us);
    }
}

}// namespace luisa::compute::fallback
