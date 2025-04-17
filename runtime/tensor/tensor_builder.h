#pragma once
#include <luisa/tensor/tensor.h>
#include <luisa/tensor/expression.h>

namespace luisa::compute {
class LC_TENSOR_API TensorBuilder {
    friend class Tensor;

    luisa::Pool<TensorData, false, false> _tensor_pool;
    luisa::vector<TensorData *> _allocated_tensor;
    ScopeExpr _root_expr;
    struct Stack {
        luisa::unique_ptr<std::byte> ptr;
        size_t size;
        size_t offset;
        Stack(
            std::byte* ptr,
            size_t size,
            size_t offset) noexcept
            : ptr(ptr), size(size), offset(offset) {}
    };
    luisa::vector<Stack> _stack_allocator;
    size_t _allocated_absolute_size = 0;
    void deallocate_tensor(TensorData *tensor) noexcept;

public:
    [[nodiscard]] luisa::span<TensorData *const> allocated_tensor() const noexcept {
        return {_allocated_tensor};
    }
    [[nodiscard]] auto &root_expr() noexcept { return _root_expr; }
    static void set_thd_local(TensorBuilder *builder);
    static TensorBuilder *get_thd_local();
    void *allocate_stack(size_t size_bytes, size_t alignment = 8) noexcept;
    template<typename T>
        requires(std::is_trivially_destructible_v<T>)
    T *allocate_array(size_t size) {
        return std::launder(reinterpret_cast<T *>(allocate_stack(size * sizeof(T), alignof(T))));
    }
    TensorData *allocate_tensor(
        luisa::span<uint32_t const> sizes,
        TensorElementType element_type) noexcept;
    template<typename T, typename... Args>
        requires(std::is_base_of_v<TensorExpr, T> && std::is_trivially_destructible_v<T> && luisa::is_constructible_v<T, uint64_t, Args && ...>)
    T *allocate_expr(Args &&...args) {
    }
    TensorBuilder() noexcept;
    ~TensorBuilder() noexcept;
    TensorBuilder(TensorBuilder const &) = delete;
    TensorBuilder(TensorBuilder &&) = delete;
};
template<typename T, typename... Args>
    requires(std::is_base_of_v<TensorExpr, T> && luisa::is_constructible_v<T, uint64_t, Args && ...>)
inline T *ScopeExpr::allocate_expr(Args &&...args) noexcept {
    auto builder = TensorBuilder::get_thd_local();
    auto ptr = static_cast<T *>(builder->allocate_stack(sizeof(T), alignof(T)));
    std::construct_at(ptr, expressions.size(), std::forward<Args>(args)...);
    expressions.emplace_back(ptr);
    return ptr;
}
}// namespace luisa::compute