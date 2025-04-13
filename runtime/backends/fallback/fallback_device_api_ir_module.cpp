//
// Created by Mike on 2024/12/10.
//

#include <luisa/core/intrin.h>
#include "fallback_device_api_ir_module.h"

#if defined(LUISA_PLATFORM_WINDOWS)

#if defined(LUISA_ARCH_X86_64)
#include "fallback_builtin/fallback_device_api_wrappers.windows.x86_64.inl"
#elif defined(LUISA_ARCH_ARM64)
#include "fallback_builtin/fallback_device_api_wrappers.windows.arm64.inl"
#else
#error Unsupported architecture on Windows
#endif

#elif defined(LUISA_PLATFORM_APPLE)

#if defined(LUISA_ARCH_X86_64)
#include "fallback_builtin/fallback_device_api_wrappers.darwin.x86_64.inl"
#elif defined(LUISA_ARCH_ARM64)
#include "fallback_builtin/fallback_device_api_wrappers.darwin.arm64.inl"
#else
#error Unsupported architecture on Windows
#endif

#elif defined(LUISA_PLATFORM_UNIX)

#if defined(LUISA_ARCH_X86_64)
#include "fallback_builtin/fallback_device_api_wrappers.linux.x86_64.inl"
#elif defined(LUISA_ARCH_ARM64)
#include "fallback_builtin/fallback_device_api_wrappers.linux.arm64.inl"
#else
#error Unsupported architecture on Windows
#endif

#endif

namespace luisa::compute::fallback {
luisa::string_view fallback_backend_device_builtin_module() noexcept {
    return {reinterpret_cast<const char *>(luisa_fallback_backend_device_builtin_module),
            std::size(luisa_fallback_backend_device_builtin_module)};
}
}// namespace luisa::compute::fallback
