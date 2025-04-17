#pragma once
#include <luisa/tensor/tensor.h>
#include <luisa/tensor/expression.h>

namespace luisa::compute {
class LC_TENSOR_API ExprTopo {
    struct ExprDependency {
        TensorExpr *self;
        // this expr depends on
        int64_t depending_count{};
        // these exprs depend on this expr
        luisa::vector<TensorExpr *> depending_self_exprs;

        ExprDependency() noexcept = default;
        ExprDependency(ExprDependency const &) = delete;
        ExprDependency(ExprDependency &&) noexcept = default;
    };
    struct TensorDataDependency {
        luisa::fixed_vector<std::pair<TensorExpr *, Usage>, 4> depend_exprs;
    };
    luisa::vector<ExprDependency> _expr_depends;
    luisa::vector<TensorDataDependency> _tensor_depends;
    // depended -> depending
    void mark_depend(
        TensorExpr *depended,
        TensorExpr *depending) noexcept;
public:
    ExprTopo(luisa::span<TensorExpr *const> exprs, size_t tensor_count) noexcept;
    luisa::vector<TensorExpr *> topo_sort()  noexcept;
    ExprTopo() noexcept;
    ExprTopo(ExprTopo const &) = delete;
    ExprTopo(ExprTopo &&) = delete;
    ~ExprTopo() noexcept;
};
}// namespace luisa::compute