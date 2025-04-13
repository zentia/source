//
// Created by Mike Smith on 2022/2/12.
//

#pragma once

#include <dsl/syntax.h>

namespace luisa::render {

using compute::Expr;
using compute::UInt;
using compute::UInt2;
using compute::UInt3;
using compute::UInt4;
using compute::Float;
using compute::ULong;

[[nodiscard]] UInt xxhash32(Expr<uint> p) noexcept;
[[nodiscard]] UInt xxhash32(Expr<uint2> p) noexcept;
[[nodiscard]] UInt xxhash32(Expr<uint3> p) noexcept;
[[nodiscard]] UInt xxhash32(Expr<uint4> p) noexcept;

[[nodiscard]] UInt pcg(Expr<uint> v) noexcept;
[[nodiscard]] UInt2 pcg2d(Expr<uint2> v_in) noexcept;
[[nodiscard]] UInt3 pcg3d(Expr<uint3> v_in) noexcept;
[[nodiscard]] UInt4 pcg4d(Expr<uint4> v_in) noexcept;

[[nodiscard]] Float uniform_uint_to_float(Expr<uint> u) noexcept;

[[nodiscard]] Float lcg(UInt &state) noexcept;

class PCG32 {

private:
    static constexpr auto default_state = 0x853c49e6748fea9bull;
    static constexpr auto default_stream = 0xda3e39cb94b95bdbull;
    static constexpr auto mult = 0x5851f42d4c957f2dull;

private:
    ULong _state;
    ULong _inc;

public:
    PCG32() noexcept;
    PCG32(ULong state, ULong inc) noexcept;
    explicit PCG32(ULong seq_index) noexcept;
    explicit PCG32(Expr<uint> seq_index) noexcept;
    void set_sequence(ULong init_seq) noexcept;
    [[nodiscard]] UInt uniform_uint() noexcept;
    [[nodiscard]] Float uniform_float() noexcept;
    [[nodiscard]] auto state() const noexcept { return _state; }
    [[nodiscard]] auto inc() const noexcept { return _inc; }
};

}// namespace luisa::render

LUISA_DISABLE_DSL_ADDRESS_OF_OPERATOR(luisa::render::PCG32)
