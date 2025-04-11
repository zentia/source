#pragma once

#include <runtime/core/stl/memory.h>
#include <runtime/core/stl/vector.h>
#include <runtime/core/stl/functional.h>
#include <runtime/core/stl/hash.h>

#if !defined(LUISA_USE_SYSTEM_STL) && !defined(LUISA_COMPUTE_USE_DENSE_MAP)
#define LUISA_COMPUTE_USE_DENSE_MAP 1
#endif

#if LUISA_COMPUTE_USE_DENSE_MAP
#include <runtime/core/stl/unordered_dense.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif

namespace luisa {

#ifdef LUISA_COMPUTE_USE_DENSE_MAP

template<typename K, typename V,
         typename Hash = luisa::hash<K>,
         typename Eq = std::equal_to<>>
using unordered_map = ankerl::unordered_dense::map<
    K, V, Hash, Eq,
    luisa::allocator<std::pair<K, V>>,
    luisa::vector<std::pair<K, V>>>;

template<typename K,
         typename Hash = luisa::hash<K>,
         typename Eq = std::equal_to<>>
using unordered_set = ankerl::unordered_dense::set<
    K, Hash, Eq, luisa::allocator<K>, luisa::vector<K>>;

#else

template<typename K, typename V,
         typename Hash = luisa::hash<K>,
         typename Eq = equal_to<>>
using unordered_map = std::unordered_map<K, V, Hash, Eq>;

template<typename K,
         typename Hash = luisa::hash<K>,
         typename Eq = equal_to<>>
using unordered_set = std::unordered_set<K, Hash, Eq>;

#endif

}// namespace luisa
