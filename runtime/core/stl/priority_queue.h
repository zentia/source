#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <queue>
#else
#include <EASTL/priority_queue.h>
#include <luisa/core/stl/vector.h>
#include <luisa/core/stl/functional.h>
#endif


namespace luisa {
#ifdef LUISA_USE_SYSTEM_STL
template<typename T,
         typename Container = std::vector<T>,
         typename Compare = std::less<>>
using priority_queue = std::priority_queue<T, Container, Compare>;
#else
template<typename T,
         typename Container = vector<T>,
         typename Compare = less<>>
using priority_queue = eastl::priority_queue<T, Container, Compare>;
#endif
}// namespace luisa
