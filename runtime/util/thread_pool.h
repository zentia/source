//
// Created by Mike Smith on 2023/5/18.
//

#pragma once

#include <future>

#include <runtime/core/stl/memory.h>
#include <runtime/core/basic_types.h>
#include <runtime/core/shared_function.h>

namespace luisa::render {

/// Thread pool class
class ThreadPool {

public:
    struct Impl;

private:
    luisa::unique_ptr<Impl> _impl;
    std::atomic_uint _task_count;

private:
    void _dispatch(luisa::SharedFunction<void()> &&task) noexcept;
    void _dispatch_all(luisa::SharedFunction<void()> &&task, size_t max_threads = std::numeric_limits<size_t>::max()) noexcept;

public:
    /// Create a thread pool with num_threads threads
    explicit ThreadPool(size_t num_threads = 0u) noexcept;
    ~ThreadPool() noexcept;
    ThreadPool(ThreadPool &&) noexcept = delete;
    ThreadPool(const ThreadPool &) noexcept = delete;
    ThreadPool &operator=(ThreadPool &&) noexcept = delete;
    ThreadPool &operator=(const ThreadPool &) noexcept = delete;
    /// Return global static ThreadPool instance
    [[nodiscard]] static bool is_worker_thread() noexcept;
    [[nodiscard]] static uint worker_thread_index() noexcept;

public:
    /// Barrier all threads
    void barrier() noexcept;
    /// Synchronize all threads
    void synchronize() noexcept;
    /// Return size of threads
    [[nodiscard]] uint size() const noexcept;
    /// Return count of tasks
    [[nodiscard]] uint task_count() const noexcept { return _task_count.load(); }

    /// Run a function async and return future of return value
    template<typename F>
        requires std::is_invocable_v<F>
    auto async(F &&f) noexcept {
        using R = std::invoke_result_t<F>;
        auto promise = luisa::make_unique<std::promise<R>>(
            std::allocator_arg, luisa::allocator{});
        auto future = promise->get_future().share();
        _task_count.fetch_add(1u);
        _dispatch([promise = std::move(promise), future, f = std::forward<F>(f), this]() mutable noexcept {
            if constexpr (std::same_as<R, void>) {
                f();
                promise->set_value();
            } else {
                promise->set_value(f());
            }
            _task_count.fetch_sub(1u);
        });
        return future;
    }

    /// Run a function parallel
    template<typename F>
        requires std::is_invocable_v<F, uint>
    void parallel(uint n, F &&f) noexcept {
        if (n == 0u) return;
        _task_count.fetch_add(1u);
        auto counter = luisa::make_unique<std::atomic_uint>(0u);
        _dispatch_all(
            [counter = std::move(counter), n, f = std::forward<F>(f), this]() mutable noexcept {
                auto i = 0u;
                while ((i = counter->fetch_add(1u)) < n) { f(i); }
                if (i == n) { _task_count.fetch_sub(1u); }
            },
            n);
    }

    /// Run a function 2D parallel
    template<typename F>
        requires std::is_invocable_v<F, uint, uint>
    void parallel(uint nx, uint ny, F &&f) noexcept {
        parallel(nx * ny, [nx, f = std::forward<F>(f)](auto i) mutable noexcept {
            f(i % nx, i / nx);
        });
    }

    /// Run a function 3D parallel
    template<typename F>
        requires std::is_invocable_v<F, uint, uint, uint>
    void parallel(uint nx, uint ny, uint nz, F &&f) noexcept {
        parallel(nx * ny * nz, [nx, ny, f = std::forward<F>(f)](auto i) mutable noexcept {
            f(i % nx, i / nx % ny, i / nx / ny);
        });
    }

    template<typename F>
        requires std::is_invocable_v<F, uint>
    auto async_parallel(uint n, F &&f) noexcept {
        auto promise = luisa::make_unique<std::promise<void>>(
            std::allocator_arg, luisa::allocator{});
        auto future = promise->get_future().share();
        if (n == 0u) {
            promise->set_value();
            return future;
        }
        _task_count.fetch_add(1u);
        auto counter = luisa::make_unique<std::pair<std::atomic_uint, std::atomic_uint>>(0u, 0u);
        _dispatch_all(
            [counter = std::move(counter), promise = std::move(promise), n, f = std::forward<F>(f), this]() mutable noexcept {
                auto i = 0u;
                auto dispatched_count = 0u;
                while ((i = counter->first.fetch_add(1u)) < n) {
                    f(i);
                    ++dispatched_count;
                }
                if (i == n) {
                    _task_count.fetch_sub(1u);
                }
                if (counter->second.fetch_add(dispatched_count) + dispatched_count == n) {
                    promise->set_value();
                }
            },
            n);
        return future;
    }

    /// Run a function 2D parallel
    template<typename F>
        requires std::is_invocable_v<F, uint, uint>
    auto async_parallel(uint nx, uint ny, F &&f) noexcept {
        return async_parallel(nx * ny, [nx, f = std::forward<F>(f)](auto i) mutable noexcept {
            f(i % nx, i / nx);
        });
    }

    /// Run a function 3D parallel
    template<typename F>
        requires std::is_invocable_v<F, uint, uint, uint>
    auto async_parallel(uint nx, uint ny, uint nz, F &&f) noexcept {
        return async_parallel(nx * ny * nz, [nx, ny, f = std::forward<F>(f)](auto i) mutable noexcept {
            f(i % nx, i / nx % ny, i / nx / ny);
        });
    }
};

[[nodiscard]] ThreadPool &global_thread_pool() noexcept;

}// namespace luisa
