#pragma once
#include <runtime/vstl/common.h>
#include <runtime/ast/function.h>
#include <runtime/runtime/rhi/argument.h>
namespace lc::hlsl {
using namespace luisa::compute;
vstd::vector<Argument> binding_to_arg(vstd::span<const Function::Binding> bindings);
}// namespace lc::hlsl
