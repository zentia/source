#pragma once

#include "Runtime/Modules/ExportModuels.h"

#define ENABLE_MEM_PROFILER (ENABLE_MEMORY_MANAGER && ENABLE_PROFILER)
#define ENABLE_AUTO_SCOPED_ROOT 1

enum MemLabelIdentifier : uint16_t
{
	kMemTempLabels,
#define DO_LABEL(Name, EnumName)
#define DO_TEMP_LABEL(Name, EnumName) EnumName,
#include "AllocatorLabelNames.h"
#undef DO_TEMP_LABEL
#undef DO_LABEL
	kMemRegularLabels,
#define DO_LABEL(Name, EnumName) EnumName,
#define DO_TEMP_LABEL(Name, EnumName) 
#include "AllocatorLabelNames.h"
#undef DO_TEMP_LABEL
#undef DO_LABEL
	kMemLabelCount,
};

struct EXPORT_COREMODULE MemLabelId
{
	constexpr MemLabelId()
		: identifier(kMemDefaultId)
	{}

	constexpr MemLabelId(MemLabelIdentifier identifier)
		: identifier(identifier)
	{
		
	}
	MemLabelIdentifier identifier;
	uint16_t salt;
	uint16_t rootReferenceIndex;
};

EXPORT_COREMODULE typedef const MemLabelId& MemLabelRef;

inline EXPORT_COREMODULE MemLabelIdentifier GetLabelIdentifier(MemLabelRef label) { return label.identifier; }

#define DO_LABEL_STRUCT(Name, EnumName) constexpr MemLabelId Name = { EnumName };
#define DO_LABEL(Name, EnumName) DO_LABEL_STRUCT(kMem##Name, EnumName)
#define DO_TEMP_LABEL(Name, EnumName) DO_LABEL_STRUCT(kMem##Name, EnumName)
#include "AllocatorLabelNames.h"
#undef DO_LABEL
#undef DO_TEMP_LABEL
#undef DO_LABEL_STRUCT