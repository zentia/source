#pragma once

#include <mutex>

#ifdef LUISA_USE_SYSTEM_STL
#include <unordered_map>
#include <list>
#else
#include <EASTL/bonus/lru_cache.h>
#endif

#include <luisa/core/stl/hash_fwd.h>
#include <luisa/core/stl/memory.h>
#include <luisa/core/stl/optional.h>

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

template<typename Key, typename Value, typename Hash = luisa::hash<Key>>
class lru_cache {

private:
    using List = std::list<Key>;
    using ListIterator = typename List::iterator;
    struct Node {
        ListIterator it;
        Value value;
    };
    using Cache = std::unordered_map<Key, Node, Hash>;

private:
    Cache _cache;
    List _list;
    luisa::function<void(const Value &)> _delete_callback;
    size_t _capacity;

private:
    void _touch(const Node &node) noexcept {
        _list.splice(_list.begin(), _list, node.it);
    }

    void _make_space() noexcept {
        while (_cache.size() >= _capacity) {
            auto iter = _cache.find(_list.back());
            if (_delete_callback) { _delete_callback(iter->second.value); }
            _cache.erase(iter);
            _list.pop_back();
        }
    }

public:
    explicit lru_cache(size_t capacity) noexcept
        : _capacity{std::max<size_t>(capacity, 1u)} {}
    void setDeleteCallback(luisa::function<void(const Value &)> callback) noexcept {
        _delete_callback = std::move(callback);
    }
    [[nodiscard]] auto at(const Key &key) noexcept -> luisa::optional<Value> {
        auto iter = _cache.find(key);
        return iter == _cache.end() ? luisa::nullopt : luisa::make_optional(iter->second.value);
    }
    auto touch(const Key &key) noexcept -> bool {
        if (auto iter = _cache.find(key); iter != _cache.end()) {
            _touch(iter->second);
            return true;
        }
        return false;
    }
    template<typename... Args>
    void emplace(const Key &key, Args &&...args) noexcept {
        if (auto iter = _cache.find(key); iter != _cache.end()) {
            _touch(iter->second);
            iter->second.value = Value{std::forward<Args>(args)...};
        } else {
            _make_space();
            _list.push_front(key);
            _cache.emplace(key, Node{_list.begin(), Value{std::forward<Args>(args)...}});
        }
    }
};

#else
template<typename Key, typename Value, typename Hash = luisa::hash<Key>>
using lru_cache = eastl::lru_cache<
    Key, Value,
    EASTLAllocatorType,
    eastl::list<Key, EASTLAllocatorType>,
    eastl::unordered_map<
        Key,
        eastl::pair<Value, typename eastl::list<Key, EASTLAllocatorType>::iterator>,
        luisa::hash<Key>,
        std::equal_to<Key>,
        EASTLAllocatorType>>;
#endif

// TODO: support allocator & comparator
template<typename Key, typename Value>
class LRUCache {

private:
    luisa::lru_cache<Key, Value> _cache;
    std::mutex _mutex;

public:
    explicit LRUCache(size_t cap) noexcept : _cache{cap} {}
    LRUCache(LRUCache &&) noexcept = delete;
    LRUCache(const LRUCache &) noexcept = delete;
    LRUCache &operator=(LRUCache &&) noexcept = delete;
    LRUCache &operator=(const LRUCache &) noexcept = delete;

    [[nodiscard]] static auto create(size_t capacity) noexcept {
        return luisa::make_unique<LRUCache>(capacity);
    }

    template<typename F>
    void set_delete_callback(F &&f) noexcept {
        _cache.setDeleteCallback(std::forward<F>(f));
    }

    [[nodiscard]] auto fetch(const Key &key) noexcept -> luisa::optional<Value> {
        std::lock_guard lock{_mutex};
        auto value = _cache.at(key);
        _cache.touch(key);
        return value;
    }

    void update(const Key &key, Value value) noexcept {
        std::lock_guard lock{_mutex};
        _cache.emplace(key, std::move(value));
    }
};

}// namespace luisa
