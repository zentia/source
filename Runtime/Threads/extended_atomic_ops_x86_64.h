#ifdef _MSC_VER
#include<windef.h>
#include<intrin.h>
#endif

#ifdef __SSE2__
#include<emmintrin.h>
#endif

static inline bool atomic_compare_exchange_strong_explicit(volatile atomic_word2* p, atomic_word2* oldval, atomic_word2 newval, int, int)
{
	return _InterlockedCompareExchange128((volatile LONGLONG*)p, (LONGLONG)newval.hi, (LONGLONG)newval.lo, (LONGLONG*)oldval) != 0;
}

static inline void atomic_store_explicit(volatile atomic_word* p, atomic_word v, memory_order_relaxed_t)
{
	*p = v;
}

static inline void atomic_store_explicit(volatile atomic_word2* p, atomic_word2 v, int)
{
	atomic_word2 c = v;
	while (!atomic_compare_exchange_strong_explicit(p, &c, v, 0, 0))
	{
		
	}
}
