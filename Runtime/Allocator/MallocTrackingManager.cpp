#include "SourcePrefix.h"
#include "MallocTrackingManager.h"

baselib::atomic<size_t> MallocTrackingManager::s_MallocLLAllocBytes = 0;