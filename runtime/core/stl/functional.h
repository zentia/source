#pragma once

#include <runtime/core/stl/type_traits.h>

#ifdef LUISA_USE_SYSTEM_STL
#include <memory>
#include <functional>
#else
#include <EASTL/finally.h>
#include <EASTL/functional.h>
#include <EASTL/bonus/adaptors.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

using std::equal_to;
using std::function;
using std::invoke;
using std::greater;
using std::greater_equal;
using std::less;
using std::less_equal;
using std::not_equal_to;

#ifdef __cpp_lib_move_only_function
using std::move_only_function;
#else
#include <luisa/core/stl/functional_impl.h>
#endif

#else

using eastl::equal_to;
using eastl::function;
using eastl::invoke;
using eastl::greater;
using eastl::greater_equal;
using eastl::less;
using eastl::less_equal;
using eastl::move_only_function;
using eastl::not_equal_to;

#endif

namespace detail {

template<typename F>
class LazyConstructor {
private:
    mutable F _ctor;

public:
    explicit LazyConstructor(F &&ctor) noexcept : _ctor{std::forward<F>(ctor)} {}
    [[nodiscard]] operator auto() const noexcept { return _ctor(); }
};

}// namespace detail

template<typename F>
[[nodiscard]] inline auto lazy_construct(F &&ctor) noexcept {
    return detail::LazyConstructor<F>(std::forward<F>(ctor));
}

#ifndef LUISA_USE_SYSTEM_STL
using eastl::make_finally;
#endif

// overloaded pattern
template<typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// deduction guide (not needed as of C++20, but provided here for compatibility)
template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename... T>
[[nodiscard]] inline auto make_overloaded(T &&...t) noexcept {
    return overloaded{std::forward<T>(t)...};
}

}// namespace luisa
