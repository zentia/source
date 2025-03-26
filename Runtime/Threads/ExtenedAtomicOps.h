#pragma once

#include "baselib/Include/Cpp/Atomic.h"
#include "extended_atomic_types.h"

#include <type_traits>

enum memory_order_relaxed_t { memory_order_relaxed = 0 };
enum memory_order_acquire_t { memory_order_acquire = 2, memory_order_consume = ::memory_order_acquire };
enum memory_order_release_t { memory_order_release = 3 };
enum memory_order_acq_rel_t { memory_order_acq_rel = 4 };
enum memory_order_seq_cst_t { memory_order_seq_cst = 5 };

#if defined(__x86_64__) || defined(_M_64)
#include "Runtime/Threads/extended_atomic_ops_x86_64.h"
#endif

__forceinline baselib::memory_order_relaxed_t TranslateMemoryOrder(memory_order_relaxed_t) { return baselib::memory_order_relaxed; }
__forceinline baselib::memory_order_acquire_t TranslateMemoryOrder(memory_order_acquire_t) { return baselib::memory_order_acquire; }
__forceinline baselib::memory_order_release_t TranslateMemoryOrder(memory_order_release_t) { return baselib::memory_order_release; }
__forceinline baselib::memory_order_acq_rel_t TranslateMemoryOrder(memory_order_acq_rel_t) { return baselib::memory_order_acq_rel; }
__forceinline baselib::memory_order_seq_cst_t TranslateMemoryOrder(memory_order_seq_cst_t) { return baselib::memory_order_seq_cst; }

#define DETAIL_SUPPRESS_ARGUMENT_DEPENDENT_LOOKUP \
	typename = typename std::enable_if< \
		std::is_same<TMemoryOrder, ::memory_order_relaxed_t>::value || \
		std::is_same<TMemoryOrder, ::memory_order_acquire_t>::value || \
		std::is_same<TMemoryOrder, ::memory_order_release_t>::value || \
		std::is_same<TMemoryOrder, ::memory_order_acq_rel_t>::value || \
		std::is_same<TMemoryOrder, ::memory_order_seq_cst_t>::value>::type

template<typename T, typename TMemoryOrder, DETAIL_SUPPRESS_ARGUMENT_DEPENDENT_LOOKUP>
__forceinline typename std::remove_cv<T>::type atomic_load_explicit(T* p, TMemoryOrder mo) { return baselib::atomic_load_explicit(*(typename  std::remove_cv<T>::type*)p, TranslateMemoryOrder(mo)); }

template<typename T, typename TMemoryOrderSuccess, typename TMemoryOrderFailure>
__forceinline bool atomic_compare_exchange_weak_explicit(T* p, typename std::remove_cv<T>::type* oldval, typename std::remove_cv<T>::type newval, TMemoryOrderSuccess success, TMemoryOrderFailure failure)
{
	return baselib::atomic_compare_exchange_weak_explicit(*(typename std::remove_cv<T>::type*)p, *oldval, newval, TranslateMemoryOrder(success), TranslateMemoryOrder(failure));
}