// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef marl_future_h
#define marl_future_h

#include <optional>
#include <chrono>
#include "conditionvariable.h"
#include "containers.h"
#include "export.h"
#include "memory.h"

#ifdef MARL_USE_SYSTEM_STL
#include <optional>
#else
#include <EASTL/optional.h>
#endif

namespace marl {

#ifdef MARL_USE_SYSTEM_STL
using std::optional;
using std::shared_ptr;
using std::is_constructible_v;
#else
using eastl::optional;
using eastl::shared_ptr;
using eastl::is_constructible_v;
#endif

// Future is a synchronization primitive used to block until a signal is raised.
template <typename T>
class Future {
 public:
  Future(Allocator* allocator = Allocator::Default);
  template <typename... Args>
    requires(marl::is_constructible_v<T, Args && ...>)
  void signal(Args&&...) const;

  // clear() clears the signaled state.
  void clear() const;

  // wait() blocks until the event is signaled.
  // If the event was constructed with the Auto Mode, then only one
  // call to wait() will unblock before returning, upon which the signalled
  // state will be automatically cleared.
  [[nodiscard]] T& wait() const;

  // test() returns true if the event is signaled, otherwise false.
  // If the event is signalled and was constructed with the Auto Mode
  // then the signalled state will be automatically cleared upon returning.
  [[nodiscard]] bool test() const;

  // isSignalled() returns true if the event is signaled, otherwise false.
  // Unlike test() the signal is not automatically cleared when the event was
  // constructed with the Auto Mode.
  // Note: No lock is held after bool() returns, so the event state may
  // immediately change after returning. Use with caution.
  [[nodiscard]] bool isSignalled() const;

 private:
  struct Shared {
    [[nodiscard]] Shared(Allocator* allocator);
    template <typename... Args>
    void signal(Args&&... args);
    T& wait();

    marl::mutex mutex;
    ConditionVariable cv;
    marl::optional<T> result;
  };

  const marl::shared_ptr<Shared> shared;
};
template <typename T>
inline Future<T>::Shared::Shared(Allocator* allocator) : cv(allocator) {}

template <typename T>
inline T& Future<T>::Shared::wait() {
  marl::lock lock(mutex);
  cv.wait(lock, [&] { return static_cast<bool>(result); });
  return *result;
}

template <typename T>
inline Future<T>::Future(Allocator* allocator /* = Allocator::Default */)
    : shared(allocator->make_shared<Shared>(allocator)) {}

template <typename T>
template <typename... Args>
  requires(marl::is_constructible_v<T, Args && ...>)
inline void Future<T>::signal(Args&&... args) const {
  shared->signal(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
inline void Future<T>::Shared::signal(Args&&... args) {
  marl::lock lock(mutex);
  result.reset();
  result.emplace(std::forward<Args>(args)...);
  cv.notify_all();
}

template <typename T>
inline void Future<T>::clear() const {
  marl::lock lock(shared->mutex);
  shared->result.reset();
}

template <typename T>
inline T& Future<T>::wait() const {
  return shared->wait();
}

template <typename T>
inline bool Future<T>::test() const {
  marl::lock lock(shared->mutex);
  if (!shared->result) {
    return false;
  }
  return true;
}

template <typename T>
inline bool Future<T>::isSignalled() const {
  marl::lock lock(shared->mutex);
  return shared->result.has_value();
}

}  // namespace marl

#endif  // marl_event_h
