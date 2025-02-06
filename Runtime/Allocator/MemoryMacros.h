#pragma once

#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/NonCopyable.h"

#define SOURCE_NEW(type, label) new (label, alignof(type), __FILE_STRIPPED__, __LINE__) type