#include <runtime/vstl/vstring.h>
#include <runtime/vstl/pool.h>
#include <runtime/core/logging.h>
#include <runtime/vstl/functional.h>
#include <runtime/vstl/memory.h>
#include <runtime/vstl/vector.h>
#include <runtime/vstl/meta_lib.h>
//#include "BinaryLinkedAllocator.h"
void *vengine_default_malloc(size_t sz) {
    return malloc(sz);
}
void vengine_default_free(void *ptr) {
    free(ptr);
}

void *vengine_default_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}
#ifdef EXPORT_UNITY_FUNCTION
VENGINE_UNITY_EXTERN void vengine_memcpy(void *dest, void *src, uint64 sz) {
    std::memcpy(dest, src, sz);
}
VENGINE_UNITY_EXTERN void vengine_memset(void *dest, byte b, uint64 sz) {
    std::memset(dest, b, sz);
}
VENGINE_UNITY_EXTERN void vengine_memmove(void *dest, void *src, uint64 sz) {
    std::memmove(dest, src, sz);
}
#endif

namespace vstd::detail{
LC_VSTL_API void one_shot_range_log_error() noexcept {
    LUISA_ERROR("This one-shot iterator has already been used, please don't do this.");
}
}
