#pragma once

#include <runtime/core/dll_export.h>
#include <runtime/core/stl/memory.h>
#include <runtime/core/stl/string.h>
#include <runtime/rust/ir.hpp>

namespace luisa::compute {
LC_IR_API void transform_ir_callable_module(ir::CallableModule *m, luisa::span<const luisa::string> transforms) noexcept;
LC_IR_API void transform_ir_kernel_module(ir::KernelModule *m, luisa::span<const luisa::string> transforms) noexcept;
LC_IR_API void transform_ir_kernel_module_auto(ir::KernelModule *m) noexcept;
}// namespace luisa::compute
