#pragma once

#include <thread>
#include <runtime/vstl/meta_lib.h>
#include <runtime/vstl/memory.h>
#include <runtime/vstl/v_allocator.h>
#include <runtime/vstl/spin_mutex.h>

namespace vstd {
template<typename T, VEngine_AllocType allocType = VEngine_AllocType::VEngine>
class LockFreeArrayQueue {
    using Allocator = VAllocHandle<allocType>;
    size_t head;
    size_t tail;
    size_t capacity;
    mutable spin_mutex mtx;
    T *arr;

    static constexpr size_t GetIndex(size_t index, size_t capacity) noexcept {
        return index & capacity;
    }
    using SelfType = LockFreeArrayQueue<T, allocType>;

public:
    LockFreeArrayQueue(size_t capacity) : head(0), tail(0) {
        if (capacity < 32) capacity = 32;
        capacity = [](size_t capacity) {
            size_t ssize = 1;
            while (ssize < capacity)
                ssize <<= 1;
            return ssize;
        }(capacity);
        this->capacity = capacity - 1;
        arr = (T *)Allocator().Malloc(sizeof(T) * capacity);
    }
    LockFreeArrayQueue(SelfType &&v)
        : head(v.head),
          tail(v.tail),
          capacity(v.capacity),
          arr(v.arr) {
        v.arr = nullptr;
    }
    void operator=(SelfType &&v) {
        this->~SelfType();
        new (this) SelfType(std::move(v));
    }
    LockFreeArrayQueue() : LockFreeArrayQueue(64) {}
    void reserve(size_t newCapa) {
        std::lock_guard<spin_mutex> lck(mtx);
        size_t index = head;
        if (newCapa > capacity) {
            auto newCapa = (capacity + 1) * 2;
            T *newArr = (T *)Allocator().Malloc(sizeof(T) * newCapa);
            newCapa--;
            for (size_t s = tail; s != index; ++s) {
                T *ptr = arr + GetIndex(s, capacity);
                new (newArr + GetIndex(s, newCapa)) T(std::move(*ptr));
                std::destroy_at(ptr);
            }
            Allocator().Free(arr);
            arr = newArr;
            capacity = newCapa;
        }
    }
    template<typename... Args>
    void push(Args &&...args) {
        std::lock_guard<spin_mutex> lck(mtx);
        size_t index = head++;
        if (head - tail > capacity) {
            auto newCapa = (capacity + 1) * 2;
            T *newArr = (T *)Allocator().Malloc(sizeof(T) * newCapa);
            newCapa--;
            for (size_t s = tail; s != index; ++s) {
                T *ptr = arr + GetIndex(s, capacity);
                new (newArr + GetIndex(s, newCapa)) T(std::move(*ptr));
                std::destroy_at(ptr);
            }
            Allocator().Free(arr);
            arr = newArr;
            capacity = newCapa;
        }
        new (arr + GetIndex(index, capacity)) T{std::forward<Args>(args)...};
    }
    template<typename... Args>
    bool try_push(Args &&...args) {
        std::unique_lock<spin_mutex> lck(mtx, std::try_to_lock);
        if (!lck.owns_lock()) return false;
        size_t index = head++;
        if (head - tail > capacity) {
            auto newCapa = (capacity + 1) * 2;
            T *newArr = (T *)Allocator().Malloc(sizeof(T) * newCapa);
            newCapa--;
            for (size_t s = tail; s != index; ++s) {
                T *ptr = arr + GetIndex(s, capacity);
                new (newArr + GetIndex(s, newCapa)) T(std::move(*ptr));
                std::destroy_at(ptr);
            }
            Allocator().Free(arr);
            arr = newArr;
            capacity = newCapa;
        }
        new (arr + GetIndex(index, capacity)) T{std::forward<Args>(args)...};
        return true;
    }
    bool pop(T *ptr) {
        std::destroy_at(ptr);
        std::lock_guard<spin_mutex> lck(mtx);
        if (head == tail)
            return false;
        auto &&value = arr[GetIndex(tail++, capacity)];
        if (std::is_trivially_move_assignable_v<T>) {
            *ptr = std::move(value);
        } else {
            new (ptr) T(std::move(value));
        }
        std::destroy_at(std::addressof(value));
        return true;
    }
    optional<T> pop() {
        mtx.lock();
        if (head == tail) {
            mtx.unlock();
            return optional<T>();
        }
        auto value = &arr[GetIndex(tail++, capacity)];
        auto disp = scope_exit([value, this]() {
            std::destroy_at(value);
            mtx.unlock();
        });
        return optional<T>(std::move(*value));
    }
    optional<T> try_pop() {
        std::unique_lock<spin_mutex> lck(mtx, std::try_to_lock);
        if (!lck.owns_lock() || head == tail) {
            return optional<T>();
        }
        auto value = &arr[GetIndex(tail++, capacity)];
        auto disp = scope_exit([value]() {
            std::destroy_at(value);
        });
        return optional<T>(std::move(*value));
    }
    ~LockFreeArrayQueue() {
        for (size_t s = tail; s != head; ++s) {
            std::destroy_at(std::addressof(arr[GetIndex(s, capacity)]));
        }
        Allocator().Free(arr);
    }
    size_t length() const {
        std::lock_guard<spin_mutex> lck(mtx);
        return head - tail;
    }
};

template<typename T, VEngine_AllocType allocType = VEngine_AllocType::VEngine>
class SingleThreadArrayQueue {
    using Allocator = VAllocHandle<allocType>;
    size_t head;
    size_t tail;
    size_t capacity;
    T *arr;

    static constexpr size_t GetIndex(size_t index, size_t capacity) noexcept {
        return index & capacity;
    }
    using SelfType = SingleThreadArrayQueue<T, allocType>;

public:
    SingleThreadArrayQueue(size_t capacity) : head(0), tail(0) {
        if (capacity < 32) capacity = 32;
        capacity = [](size_t capacity) {
            size_t ssize = 1;
            while (ssize < capacity)
                ssize <<= 1;
            return ssize;
        }(capacity);
        this->capacity = capacity - 1;
        arr = (T *)Allocator().Malloc(sizeof(T) * capacity);
    }
    SingleThreadArrayQueue(SelfType &&v)
        : head(v.head),
          tail(v.tail),
          capacity(v.capacity),
          arr(v.arr) {
        v.arr = nullptr;
    }
    void operator=(SelfType &&v) {
        this->~SelfType();
        new (this) SelfType(std::move(v));
    }
    SingleThreadArrayQueue() : SingleThreadArrayQueue(64) {}
    void reserve(size_t newCapa) {
        size_t index = head;
        if (newCapa > capacity) {
            auto newCapa = (capacity + 1) * 2;
            T *newArr = (T *)Allocator().Malloc(sizeof(T) * newCapa);
            newCapa--;
            for (size_t s = tail; s != index; ++s) {
                T *ptr = arr + GetIndex(s, capacity);
                new (newArr + GetIndex(s, newCapa)) T(std::move(*ptr));
                std::destroy_at(ptr);
            }
            Allocator().Free(arr);
            arr = newArr;
            capacity = newCapa;
        }
    }
    template<typename... Args>
    T *push(Args &&...args) {
        size_t index = head++;
        if (head - tail > capacity) {
            auto newCapa = (capacity + 1) * 2;
            T *newArr = (T *)Allocator().Malloc(sizeof(T) * newCapa);
            newCapa--;
            for (size_t s = tail; s != index; ++s) {
                T *ptr = arr + GetIndex(s, capacity);
                new (newArr + GetIndex(s, newCapa)) T(std::move(*ptr));
                std::destroy_at(ptr);
            }
            Allocator().Free(arr);
            arr = newArr;
            capacity = newCapa;
        }
        return new (arr + GetIndex(index, capacity)) T{std::forward<Args>(args)...};
    }
    T *front() {
        if (head == tail)
            return nullptr;
        auto &&value = arr[GetIndex(tail, capacity)];
        return &value;
    }
    bool pop(T *ptr) {
        std::destroy_at(ptr);
        if (head == tail)
            return false;
        auto &&value = arr[GetIndex(tail++, capacity)];
        if (std::is_trivially_move_assignable_v<T>) {
            *ptr = std::move(value);
        } else {
            new (ptr) T(std::move(value));
        }
        std::destroy_at(std::addressof(value));
        return true;
    }
    optional<T> pop() {
        if (head == tail) {
            return optional<T>();
        }
        auto value = &arr[GetIndex(tail++, capacity)];
        auto disp = scope_exit([value]() {
            std::destroy_at(value);
        });
        return optional<T>(std::move(*value));
    }
    void pop_discard() {
        if (head == tail) {
            return;
        }
        auto value = &arr[GetIndex(tail++, capacity)];
        std::destroy_at(value);
    }
    ~SingleThreadArrayQueue() {
        for (size_t s = tail; s != head; ++s) {
            std::destroy_at(std::addressof(arr[GetIndex(s, capacity)]));
        }
        Allocator().Free(arr);
    }
    size_t length() const {
        return head - tail;
    }
};
}// namespace vstd
