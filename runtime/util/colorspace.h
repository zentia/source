//
// Created by Mike Smith on 2022/1/19.
//

#pragma once

#include <core/basic_types.h>
#include <dsl/syntax.h>

namespace luisa::render {

template<typename T>
[[nodiscard]] inline auto cie_xyz_to_linear_srgb(T &&xyz) noexcept {
    constexpr auto m = make_float3x3(
        +3.240479f, -0.969256f, +0.055648f,
        -1.537150f, +1.875991f, -0.204043f,
        -0.498535f, +0.041556f, +1.057311f);
    return m * std::forward<T>(xyz);
}

template<typename T>
[[nodiscard]] inline auto linear_srgb_to_cie_y(T &&rgb) noexcept {
    constexpr auto m = make_float3(0.212671f, 0.715160f, 0.072169f);
    return dot(m, std::forward<T>(rgb));
}

template<typename T>
[[nodiscard]] inline auto linear_srgb_to_cie_xyz(T &&rgb) noexcept {
    constexpr auto m = make_float3x3(
        0.412453f, 0.212671f, 0.019334f,
        0.357580f, 0.715160f, 0.119193f,
        0.180423f, 0.072169f, 0.950227f);
    return m * std::forward<T>(rgb);
}

template<typename T>
[[nodiscard]] inline auto cie_xyz_to_lab(T &&xyz) noexcept {
    static constexpr auto delta = 6.f / 29.f;
    auto f = [](const auto &x) noexcept {
        constexpr auto d3 = delta * delta * delta;
        constexpr auto one_over_3d2 = 1.f / (3.f * delta * delta);
        if constexpr (compute::is_dsl_v<T>) {
            return compute::ite(x > d3, compute::pow(x, 1.f / 3.f), one_over_3d2 * x + (4.f / 29.f));
        } else {
            return x > d3 ? std::pow(x, 1.f / 3.f) : one_over_3d2 * x + 4.f / 29.f;
        }
    };
    auto f_y_yn = f(xyz.y);
    auto l = 116.f * f_y_yn - 16.f;
    auto a = 500.f * (f(xyz.x / .950489f) - f_y_yn);
    auto b = 200.f * (f_y_yn - f(xyz.z / 1.08884f));
    return make_float3(l, a, b);
}

template<typename T>
[[nodiscard]] inline auto lab_to_cie_xyz(T &&lab) noexcept {
    static constexpr auto delta = 6.f / 29.f;
    auto f = [](const auto &x) noexcept {
        constexpr auto three_dd = 3.f * delta * delta;
        if constexpr (compute::is_dsl_v<T>) {
            return compute::ite(x > delta, x * x * x, three_dd * x - (three_dd * 4.f / 29.f));
        } else {
            return x > delta ? x * x * x : three_dd * x - three_dd * 4.f / 29.f;
        }
    };
    auto v = (lab.x + 16.f) / 116.f;
    auto x = .950489f * f(v + lab.y / 500.f);
    auto y = f(v);
    auto z = 1.08884f * f(v - lab.z / 200.f);
    return make_float3(x, y, z);
}

}// namespace luisa::render
