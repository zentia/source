#include <runtime/core/logging.h>
#include <runtime/core/stl/memory.h>

namespace luisa::detail {

namespace {
[[nodiscard]] inline auto is_alignment_compatible_with_malloc(size_t alignment) noexcept {
    return (alignment == 0u ? 16u : alignment) <= alignof(std::max_align_t);
}
}// namespace

LUISA_EXPORT_API void *allocator_allocate(size_t size, size_t alignment) noexcept {
#ifdef LUISA_USE_SYSTEM_STL
#ifndef _WIN32
    if (is_alignment_compatible_with_malloc(alignment)) {
        return ::malloc(size);
    }
#endif
    alignment = std::max<size_t>(alignment, 16u);
    size = luisa::align(size, alignment);
#ifdef _WIN32
    auto p = _aligned_malloc(size, alignment);
#else
    auto p = ::aligned_alloc(alignment, size);
#endif
    LUISA_DEBUG_ASSERT(p != nullptr, "Failed to allocate memory.");
    return p;
#else
    return eastl::GetDefaultAllocator()->allocate(size, alignment, 0u);
#endif
}

LUISA_EXPORT_API void allocator_deallocate(void *p, size_t) noexcept {
#ifdef LUISA_USE_SYSTEM_STL
#ifdef _WIN32
    return _aligned_free(p);
#else
    return ::free(p);
#endif
#else
    eastl::GetDefaultAllocator()->deallocate(p, 0u);
#endif
}

LUISA_EXPORT_API void *allocator_reallocate(void *p, size_t size, size_t alignment) noexcept {
#ifdef LUISA_USE_SYSTEM_STL
#ifdef _WIN32
    alignment = std::max<size_t>(alignment, 16u);
    size = luisa::align(size, alignment);
    return _aligned_realloc(p, size, alignment);
#else
    if (is_alignment_compatible_with_malloc(alignment)) {
        return ::realloc(p, size);
    }
    auto new_p = allocator_allocate(size, alignment);
    std::memcpy(new_p, p, size);
    allocator_deallocate(p, 0u);
    return new_p;
#endif
#else
    p = eastl::GetDefaultAllocator()->reallocate(p, size);
    LUISA_DEBUG_ASSERT(alignment == 0u || reinterpret_cast<uintptr_t>(p) % alignment == 0u,
                       "Reallocation failed to maintain alignment.");
    return p;
#endif
}

}// namespace luisa::detail
