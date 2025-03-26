#include "SourcePrefix.h"
#include "atomic_queue.h"
#include "ExtenedAtomicOps.h"
#include "Cpp/Atomic.h"

atomic_stack::atomic_stack()
{
	atomic_word2 w;
	w.lo = w.hi = 0;
	atomic_store_explicit(&top_, w, memory_order_relaxed);
}

void atomic_stack::push(atomic_node* node)
{
	atomic_word2 top = atomic_load_explicit(&top_, memory_order_relaxed);
	atomic_word2 new_top;
	new_top.lo = (atomic_word)node;
	do
	{
		atomic_store_explicit(&node->next_, top.lo, memory_order_relaxed);
	}
	while (!atomic_compare_exchange_strong_explicit(&top_, &top, new_top, memory_order_release, memory_order_relaxed));
}
