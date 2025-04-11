#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <stack>
#include <vector>
#else
#include <EASTL/stack.h>
#include <luisa/core/stl/vector.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL
template<typename T, typename Container = std::vector<T>>
using stack = std::stack<T, Container>;
#else
template<typename T, typename Container = luisa::vector<T>>
using stack = eastl::stack<T, Container>;
#endif

}// namespace luisa
