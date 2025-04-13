#pragma once

#include <luisa/core/stl/string.h>

namespace luisa::compute::fallback {
[[nodiscard]] luisa::string_view fallback_backend_device_builtin_module() noexcept;
}// namespace luisa::compute::fallback
