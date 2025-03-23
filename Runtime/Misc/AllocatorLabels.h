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

struct AllocationRootWithSalt
{
	uint16_t m_Salt;
	uint32_t m_RootReferenceIndex;

	static const AllocationRootWithSalt kNoRoot;
	static constexpr uint32_t kNotInList = (uint32_t)-1;
};

struct EXPORT_COREMODULE MemLabelId
{
	constexpr MemLabelId()
		: identifier(kMemDefaultId)
		, salt(0)
		, rootReferenceIndex(AllocationRootWithSalt::kNotInList)
	{}

	constexpr MemLabelId(int64_t) = delete;
	constexpr MemLabelId(uint64_t) = delete;

	constexpr MemLabelId(MemLabelIdentifier identifier)
		: identifier(identifier)
		, salt(0)
		, rootReferenceIndex(AllocationRootWithSalt::kNotInList)
	{

	}

	constexpr MemLabelId(MemLabelIdentifier identifier, AllocationRootWithSalt rootReferenceWithSalt)
		: identifier(identifier)
		, salt(rootReferenceWithSalt.m_Salt)
		, rootReferenceIndex(rootReferenceWithSalt.m_RootReferenceIndex)
	{}
	MemLabelIdentifier identifier;
	uint16_t salt;
	uint16_t rootReferenceIndex;
};

typedef const MemLabelId& MemLabelRef;

inline MemLabelIdentifier GetLabelIdentifier(MemLabelRef label) { return label.identifier; }
inline bool IsTempLabel(MemLabelRef label) { return GetLabelIdentifier(label) < kMemRegularLabels; }

inline MemLabelId CreateMemLabel(MemLabelIdentifier id) { return { id, AllocationRootWithSalt::kNoRoot }; }

MemLabelId SetCurrentMemoryOwner(MemLabelRef label);

#define DO_LABEL_STRUCT(Name, EnumName) constexpr MemLabelId Name = { EnumName };
#define DO_LABEL(Name, EnumName) DO_LABEL_STRUCT(kMem##Name, EnumName)
#define DO_TEMP_LABEL(Name, EnumName) DO_LABEL_STRUCT(kMem##Name, EnumName)
#include "AllocatorLabelNames.h"
#undef DO_LABEL
#undef DO_TEMP_LABEL
#undef DO_LABEL_STRUCT