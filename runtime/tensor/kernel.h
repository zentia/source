#pragma once
#include <luisa/tensor/expression.h>
#include <luisa/tensor/tensor_builder.h>
#include <luisa/runtime/device.h>
#include <luisa/vstl/meta_lib.h>
namespace luisa::compute {
namespace detail {
template<typename Ret, typename... Args>
struct ObjType {
    template<template<typename...> typename Object, typename... ExtraTypes>
    using Type = typename Object<ExtraTypes..., Ret, Args...>;
};
template<typename T>
struct member_func_meta;

template<typename Ret, typename Class, typename... Args>
struct member_func_meta<Ret (Class::*)(Args...) const> : ObjType<Ret, Args...> {
    static constexpr bool is_const = true;
};
template<typename Ret, typename Class, typename... Args>
struct member_func_meta<Ret (Class::*)(Args...)> : ObjType<Ret, Args...> {
    static constexpr bool is_const = false;
};
template<typename Ret, typename Class, typename... Args>
struct member_func_meta<Ret (Class::*)(Args...) const noexcept> : ObjType<Ret, Args...> {
    static constexpr bool is_const = true;
};
template<typename Ret, typename Class, typename... Args>
struct member_func_meta<Ret (Class::*)(Args...) noexcept> : ObjType<Ret, Args...> {
    static constexpr bool is_const = false;
};
}// namespace detail

struct TensorDescriptor {
    luisa::fixed_vector<uint32_t, 4> _sizes;
    TensorElementType _type;
    TensorDescriptor(
        std::initializer_list<uint32_t> dimensions,
        TensorElementType type) noexcept : _type(type) {
        _sizes.resize_uninitialized(dimensions.size());
        std::memcpy(_sizes.data(), dimensions.begin(), _sizes.size_bytes());
    }
    TensorDescriptor(
        luisa::span<uint32_t const> dimensions,
        TensorElementType type) noexcept : _type(type) {
        _sizes.resize_uninitialized(dimensions.size());
        std::memcpy(_sizes.data(), dimensions.data(), _sizes.size_bytes());
    }
};

namespace detail {
template<typename T>
struct tensor_kernel_type {
    using Type = T;
    static T &&_forward(T &&v) noexcept {
        return std::forward<T>(v);
    }
};
template<>
struct tensor_kernel_type<Tensor> {
    using Type = TensorDescriptor;
    static Tensor _forward(TensorDescriptor &&v) noexcept {
        return Tensor{TensorBuilder::get_thd_local()->allocate_tensor(v._sizes, v._type)};
    }
};
}// namespace detail

template<typename Lambda, typename Ret, typename... Args>
struct TensorKernel {
    static_assert(luisa::always_false_v<Ret>, "Tensor kernel must not have return value.");
};

template<typename Lambda, typename... Args>
class TensorKernel<Lambda, void, Args...> {
    Lambda lambda;
public:
    explicit TensorKernel(Lambda &&lambda) noexcept : lambda(std::forward<Lambda>(lambda)) {}
    luisa::unique_ptr<TensorBuilder> compile(
        // Device const& device,
        typename detail::tensor_kernel_type<Args>::Type... args) noexcept {
        luisa::unique_ptr<TensorBuilder> r = luisa::make_unique<TensorBuilder>();
        TensorBuilder::set_thd_local(r.get());
        lambda(detail::tensor_kernel_type<Args>::_forward(std::forward<typename detail::tensor_kernel_type<Args>::Type>(args))...);
        TensorBuilder::set_thd_local(nullptr);
        // TODO: dispatch backend
        return r;
    }
};

template<typename Lambda>
static auto load_tensor(Lambda &&lambda) noexcept {
    using KernelType = typename detail::member_func_meta<decltype(&std::remove_cvref_t<decltype(lambda)>::operator())>::Type<TensorKernel, Lambda>;
    return KernelType{std::forward<Lambda>(lambda)};
}

}// namespace luisa::compute