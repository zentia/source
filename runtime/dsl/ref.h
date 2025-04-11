#pragma once

#include <runtime/dsl/expr.h>

namespace luisa::compute {

template<typename T>
struct Var;

inline namespace dsl {
/// Assign rhs to lhs
template<typename Lhs, typename Rhs>
void assign(Lhs &&lhs, Rhs &&rhs) noexcept;// defined in dsl/stmt.h
}// namespace dsl

namespace detail {

/// Enable subscript access
template<typename T>
struct RefEnableSubscriptAccess {
    /// Access index
    template<typename I>
        requires is_integral_expr_v<I>
    [[nodiscard]] auto &operator[](I &&index) & noexcept {
        using Elem = std::remove_cvref_t<
            decltype(std::declval<expr_value_t<T>>()[0])>;
        auto f = FunctionBuilder::current();
        auto expr = f->access(
            Type::of<Elem>(),
            static_cast<const T *>(this)->expression(),
            extract_expression(std::forward<I>(index)));
        return *f->create_temporary<Var<Elem>>(expr);
    }
    /// Access index
    template<typename I>
        requires is_integral_expr_v<I>
    [[nodiscard]] const auto &operator[](I &&index) const & noexcept {
        return (*const_cast<RefEnableSubscriptAccess *>(this))[std::forward<I>(index)];
    }
};

/// Enbale get member by index
template<typename T>
struct RefEnableGetMemberByIndex {
    /// Get member by index
    template<size_t i>
    [[nodiscard]] const auto &get() const noexcept {
        static_assert(i < dimension_v<expr_value_t<T>>);
        auto self = const_cast<T *>(static_cast<const T *>(this));
        return (*self)[static_cast<uint>(i)];
    }
};

template<typename T>
struct RefEnableGetAddress {
    // see definition in var.h as we need to specialize Var<ulong> for address()
    [[nodiscard]] Var<ulong> address() const noexcept;
};

/// Ref class common definition
#define LUISA_REF_COMMON(...)                                              \
private:                                                                   \
    const Expression *_expression;                                         \
                                                                           \
public:                                                                    \
    explicit Ref(const Expression *e) noexcept : _expression{e} {}         \
    [[nodiscard]] auto expression() const noexcept { return _expression; } \
    Ref(Ref &&) noexcept = default;                                        \
    Ref(Var<__VA_ARGS__> &&other) noexcept                                 \
        : Ref{static_cast<Ref &&>(other)} {}                               \
    Ref(const Ref &) noexcept = default;                                   \
    template<typename Rhs>                                                 \
    void operator=(Rhs &&rhs) & noexcept {                                 \
        dsl::assign(*this, std::forward<Rhs>(rhs));                        \
    }                                                                      \
    [[nodiscard]] operator Expr<__VA_ARGS__>() const noexcept {            \
        return Expr<__VA_ARGS__>{this->expression()};                      \
    }                                                                      \
    void operator=(Ref rhs) & noexcept { (*this) = Expr<__VA_ARGS__>{rhs}; }

/// Ref<T>
template<typename T>
struct Ref
    : detail::ExprEnableStaticCast<Ref<T>>,
      detail::ExprEnableBitwiseCast<Ref<T>>,
      detail::RefEnableGetAddress<Ref<T>> {
    static_assert(concepts::scalar<T>);
    LUISA_REF_COMMON(T)
    template<size_t i>
    [[nodiscard]] const auto &get() const noexcept {
        static_assert(i == 0u);
        return *this;
    }
};

/// Ref<std::array<T, N>>
template<typename T, size_t N>
struct Ref<std::array<T, N>>
    : detail::ExprEnableBitwiseCast<Ref<std::array<T, N>>>,
      detail::RefEnableSubscriptAccess<Ref<std::array<T, N>>>,
      detail::RefEnableGetMemberByIndex<Ref<std::array<T, N>>>,
      detail::RefEnableGetAddress<Ref<std::array<T, N>>> {
    LUISA_REF_COMMON(std::array<T, N>)
};

/// Ref<std::array<T, N>>
template<typename T, size_t N>
struct Ref<T[N]>
    : detail::ExprEnableBitwiseCast<Ref<T[N]>>,
      detail::RefEnableSubscriptAccess<Ref<T[N]>>,
      detail::RefEnableGetMemberByIndex<Ref<T[N]>>,
      detail::RefEnableGetAddress<Ref<T[N]>> {
    LUISA_REF_COMMON(T[N])
};

/// Ref<Matrix<N>>
template<size_t N>
struct Ref<Matrix<N>>
    : detail::ExprEnableBitwiseCast<Ref<Matrix<N>>>,
      detail::RefEnableSubscriptAccess<Ref<Matrix<N>>>,
      detail::RefEnableGetMemberByIndex<Ref<Matrix<N>>>,
      detail::RefEnableGetAddress<Ref<Matrix<N>>> {
    LUISA_REF_COMMON(Matrix<N>)
};

/// Ref<std::tuple<T...>>
template<typename... T>
struct Ref<std::tuple<T...>> {
    LUISA_REF_COMMON(std::tuple<T...>)
    template<size_t i>
    [[nodiscard]] const auto &get() const noexcept {
        using M = std::tuple_element_t<i, std::tuple<T...>>;
        auto fb = detail::FunctionBuilder::current();
        auto expr = fb->member(Type::of<M>(), this->expression(), i);
        return *fb->create_temporary<Var<M>>(expr);
    }
};

/// Ref<Vector<T, 2>>
template<typename T>
struct Ref<Vector<T, 2>>
    : detail::ExprEnableStaticCast<Ref<Vector<T, 2>>>,
      detail::ExprEnableBitwiseCast<Ref<Vector<T, 2>>>,
      detail::RefEnableSubscriptAccess<Ref<Vector<T, 2>>>,
      detail::RefEnableGetMemberByIndex<Ref<Vector<T, 2>>>,
      detail::RefEnableGetAddress<Ref<Vector<T, 2>>> {
    LUISA_REF_COMMON(Vector<T, 2>)
    Var<T> x{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Var<T> y{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
#include <runtime/dsl/swizzle_2.inl.h>
};

/// Ref<Vector<T, 3>>
template<typename T>
struct Ref<Vector<T, 3>>
    : detail::ExprEnableStaticCast<Ref<Vector<T, 3>>>,
      detail::ExprEnableBitwiseCast<Ref<Vector<T, 3>>>,
      detail::RefEnableSubscriptAccess<Ref<Vector<T, 3>>>,
      detail::RefEnableGetMemberByIndex<Ref<Vector<T, 3>>>,
      detail::RefEnableGetAddress<Ref<Vector<T, 3>>> {
    LUISA_REF_COMMON(Vector<T, 3>)
    Var<T> x{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Var<T> y{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
    Var<T> z{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x2u)};
#include <runtime/dsl/swizzle_3.inl.h>
};

/// Ref<Vector<T, 4>>
template<typename T>
struct Ref<Vector<T, 4>>
    : detail::ExprEnableStaticCast<Ref<Vector<T, 4>>>,
      detail::ExprEnableBitwiseCast<Ref<Vector<T, 4>>>,
      detail::RefEnableSubscriptAccess<Ref<Vector<T, 4>>>,
      detail::RefEnableGetMemberByIndex<Ref<Vector<T, 4>>>,
      detail::RefEnableGetAddress<Ref<Vector<T, 4>>> {
    LUISA_REF_COMMON(Vector<T, 4>)
    Var<T> x{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Var<T> y{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
    Var<T> z{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x2u)};
    Var<T> w{detail::FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x3u)};
#include <runtime/dsl/swizzle_4.inl.h>
};

#undef LUISA_REF_COMMON

}// namespace detail

}// namespace luisa::compute
