#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#else
#include <EASTL/fixed_map.h>
#include <EASTL/fixed_set.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_hash_set.h>
#endif

#include <luisa/core/stl/hash.h>
#include <luisa/core/stl/functional.h>

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Hash = luisa::hash<Key>, typename Eq = std::less<>>
using fixed_unordered_map = std::unordered_map<Key, Value, Hash, Eq>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Hash = luisa::hash<Key>, typename Eq = std::less<>>
using fixed_unordered_set = std::unordered_set<Key, Hash, Eq>;

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_map = std::map<Key, Value, Compare>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_set = std::set<Key, Compare>;

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_multimap = std::multimap<Key, Value, Compare>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_multiset = std::multiset<Key, Compare>;

#else

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Hash = hash<Key>, typename Eq = equal_to<>>
using fixed_unordered_map = eastl::fixed_hash_map<
    Key, Value, node_count, node_count + 1u, allow_overflow, Hash, Eq>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Hash = hash<Key>, typename Eq = equal_to<>>
using fixed_unordered_set = eastl::fixed_hash_set<
    Key, node_count, node_count + 1u, allow_overflow, Hash, Eq>;

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_map = eastl::fixed_map<
    Key, Value, node_count, allow_overflow, Compare>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_set = eastl::fixed_set<
    Key, node_count, allow_overflow, Compare>;

template<typename Key, typename Value,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_multimap = eastl::fixed_multimap<
    Key, Value, node_count, allow_overflow, Compare>;

template<typename Key,
         size_t node_count, bool allow_overflow = true,
         typename Compare = std::less<>>
using fixed_multiset = eastl::fixed_multiset<
    Key, node_count, allow_overflow, Compare>;

#endif

}// namespace luisa
