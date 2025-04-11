#pragma once

#include <new>
#include <utility>
#include <type_traits>

#ifndef LUISA_USE_SYSTEM_STL
#include <EASTL/internal/type_pod.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL
using std::is_constructible_v;
#else
template<class T, class... Args>
constexpr bool is_constructible_v = eastl::is_constructible_v<T, Args...>;
#endif

}// namespace luisa
