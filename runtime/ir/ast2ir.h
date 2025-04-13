#pragma once

#include <runtime/ast/variable.h>
#include <runtime/ast/expression.h>
#include <runtime/ast/statement.h>
#include <runtime/ast/function.h>
#include <runtime/core/stl/unordered_map.h>

#include <runtime/rust/ir.hpp>

namespace luisa::compute {

namespace detail {
class FunctionBuilder;
}// namespace detail

class LC_IR_API AST2IR {
    
public:
    [[nodiscard]] static luisa::shared_ptr<ir::CArc<ir::KernelModule>> build_kernel(Function function) noexcept;
    [[nodiscard]] static luisa::shared_ptr<ir::CArc<ir::CallableModule>> build_callable(Function function) noexcept;
    [[nodiscard]] static ir::CArc<ir::Type> build_type(const Type *type) noexcept;
};

}// namespace luisa::compute
