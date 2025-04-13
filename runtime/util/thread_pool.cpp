//
// Created by Mike Smith on 2023/5/18.
//

#include <version>
#include <sstream>
#include <thread>
#include <memory>
#include <condition_variable>

#if (!defined(__clang_major__) || __clang_major__ >= 14) && defined(__cpp_lib_barrier)
#define LUISA_COMPUTE_USE_STD_BARRIER
#endif

#ifdef LUISA_COMPUTE_USE_STD_BARRIER
#include <barrier>
#endif

#include <runtime/core/stl/vector.h>
#include <runtime/core/stl/queue.h>
#include <runtime/core/logging.h>
#include <util/thread_pool.h>

namespace luisa::render {

namespace detail {

[[nodiscard]] static auto &is_worker_thread() noexcept {
    static thread_local auto is_worker = false;
    return is_worker;
}

[[nodiscard]] static auto &worker_thread_index() noexcept {
    static thread_local auto id = 0u;
    return id;
}

static inline void check_not_in_worker_thread(std::string_view f) noexcept {
    if (is_worker_thread()) [[unlikely]] {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        LUISA_ERROR_WITH_LOCATION(
            "Invoking ThreadPool::{}() "
            "from worker thread {}.",
            f, oss.str());
    }
}

}// namespace detail

#ifdef LUISA_COMPUTE_USE_STD_BARRIER
struct Barrier : std::barrier<> {
    using std::barrier<>::barrier;
};
#else
// reference: https://github.com/yohhoy/yamc/blob/master/include/yamc_barrier.hpp
class Barrier {
private:
    uint _n;
    uint _counter;
    uint _phase;
    std::condition_variable _cv;
    std::mutex _callback_mutex;

public:
    explicit Barrier(uint n) noexcept
        : _n{n}, _counter{n}, _phase{0u} {}
    void arrive_and_wait() noexcept {
        std::unique_lock lock{_callback_mutex};
        auto arrive_phase = _phase;
        if (--_counter == 0u) {
            _counter = _n;
            _phase++;
            _cv.notify_all();
        }
        while (_phase <= arrive_phase) {
            _cv.wait(lock);
        }
    }
};
#endif

struct ThreadPool::Impl {
    luisa::vector<std::thread> threads;
    luisa::queue<luisa::SharedFunction<void()>> tasks;
    std::mutex mutex;
    Barrier synchronize_barrier;
    Barrier dispatch_barrier;
    std::condition_variable cv;
    bool should_stop{false};
    explicit Impl(size_t num_threads) noexcept
        : synchronize_barrier(num_threads + 1u),
          dispatch_barrier(num_threads) {
        threads.reserve(num_threads);
        for (auto i = 0u; i < num_threads; i++) {
            threads.emplace_back([this, i] {
                detail::is_worker_thread() = true;
                detail::worker_thread_index() = i;
                for (;;) {
                    std::unique_lock lock{mutex};
                    cv.wait(lock, [this] { return !tasks.empty() || should_stop; });
                    if (should_stop && tasks.empty()) [[unlikely]] { break; }
                    auto task = std::move(tasks.front());
                    tasks.pop();
                    lock.unlock();
                    task();
                }
            });
        }
        LUISA_INFO("Created thread pool with {} thread{}.",
                   num_threads, num_threads == 1u ? "" : "s");
    }
};

ThreadPool::ThreadPool(size_t num_threads) noexcept
    : _impl{luisa::make_unique<Impl>([&]() {
          if (num_threads == 0u) {
              num_threads = std::max(
                  std::thread::hardware_concurrency(), 1u);
          }
          return num_threads;
      }())} {
}

void ThreadPool::barrier() noexcept {
    detail::check_not_in_worker_thread("barrier");
    _dispatch_all([this] { _impl->dispatch_barrier.arrive_and_wait(); });
}

void ThreadPool::synchronize() noexcept {
    detail::check_not_in_worker_thread("synchronize");
    while (task_count() != 0u) {
        _dispatch_all([this] { _impl->synchronize_barrier.arrive_and_wait(); });
        _impl->synchronize_barrier.arrive_and_wait();
    }
}

void ThreadPool::_dispatch(luisa::SharedFunction<void()> &&task) noexcept {
    {
        std::lock_guard lock{_impl->mutex};
        _impl->tasks.emplace(std::move(task));
    }
    _impl->cv.notify_one();
}

void ThreadPool::_dispatch_all(luisa::SharedFunction<void()> &&task, size_t max_threads) noexcept {
    {
        std::lock_guard lock{_impl->mutex};
        for (auto i = 0u; i < std::min(_impl->threads.size(), max_threads) - 1u; i++) {
            _impl->tasks.emplace(task);
        }
        _impl->tasks.emplace(std::move(task));
    }
    _impl->cv.notify_all();
}

ThreadPool::~ThreadPool() noexcept {
    {
        std::lock_guard lock{_impl->mutex};
        _impl->should_stop = true;
    }
    _impl->cv.notify_all();
    for (auto &&t : _impl->threads) { t.join(); }
}

uint ThreadPool::size() const noexcept {
    return static_cast<uint>(_impl->threads.size());
}
bool ThreadPool::is_worker_thread() noexcept {
    return detail::is_worker_thread();
}
uint ThreadPool::worker_thread_index() noexcept {
    LUISA_ASSERT(detail::is_worker_thread(),
                 "ThreadPool::worker_thread_index() "
                 "called in non-worker thread.");
    return detail::worker_thread_index();
}

ThreadPool &global_thread_pool() noexcept {
    static ThreadPool pool{std::thread::hardware_concurrency()};
    return pool;
}

}// namespace luisa
