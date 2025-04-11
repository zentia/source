#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <queue>
#else
#include <EASTL/queue.h>
#include <runtime/core/stl/deque.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL
using std::queue;
#else
template<typename T, typename Container = luisa::deque<T>>
using queue = eastl::queue<T, Container>;
#endif

}// namespace luisa
