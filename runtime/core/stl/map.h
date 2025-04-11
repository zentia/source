#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <map>
#include <set>
#else
#include <EASTL/map.h>
#include <EASTL/set.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

template<typename Key,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<Key>>
using set = std::set<Key, Compare, allocator>;

template<typename Key, typename Value,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<std::pair<const Key, Value>>>
using map = std::map<Key, Value, Compare, allocator>;

template<typename Key,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<Key>>
using multiset = std::multiset<Key, Compare, allocator>;

template<typename Key, typename Value,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<std::pair<const Key, Value>>>
using multimap = std::multimap<Key, Value, Compare, allocator>;

#else

template<typename Key,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<Key>>
using set = eastl::set<Key, Compare, allocator>;

template<typename Key, typename Value,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<std::pair<const Key, Value>>>
using map = eastl::map<Key, Value, Compare, allocator>;

template<typename Key,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<Key>>
using multiset = eastl::multiset<Key, Compare, allocator>;

template<typename Key, typename Value,
         typename Compare = std::less<>,
         typename allocator = luisa::allocator<std::pair<const Key, Value>>>
using multimap = eastl::multimap<Key, Value, Compare, allocator>;

#endif

}// namespace luisa
