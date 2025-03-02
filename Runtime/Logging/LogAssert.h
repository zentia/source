#pragma once

#include "Runtime/BaseClasses/InstanceID.h"
#include "Runtime/Modules/ExportModuels.h"
#include "Runtime/Utitlities/StringTraits.h"

enum LogMessageFlags
{
	kNoLogMessageFlags = 0,
	kError = 1 << 0,
	kAssert = 1 << 1,
	kLog = 1 << 2,
	kFatal = 1 << 3,
	kAssetImportError = 1 << 4,

};

enum LogType
{
	kLogTypeError = 0,
	kLogTypeAssert = 1,
	kLogTypeWarning = 2,
	kLogTypeLog = 3,
	kLogTypeException = 4,
	kLogTypeDebug = 5,
	kLogTypeNumLevels
};

struct DebugStringToFileData
{
	const char* message;
	LogMessageFlags mode;

	DebugStringToFileData()
		: message("")
		, mode(kNoLogMessageFlags)
	{}
};

EXPORT_COREMODULE void DebugStringToFile(const DebugStringToFileData& data);

template<typename TString>
void DebugStringToFile(const TString& message, const char* file, int line, int column, LogMessageFlags mode, const InstanceID objectInstanceID = InstanceID_None, int identifier = 0)
{
	DebugStringToFileData data;
	data.message = StringTraits::AsConstTChars(message);
	data.mode = mode;
	DebugStringToFile(data);
}

#define NON_SOURCE_RELEASE_ONLY(code) code

bool AssertImplementation(InstanceID objID, const char* fileStripped, int line, int column, const char* msg);

#define SOURCE__ASSERT_IMPL(test, objID, msg) PP_WRAP_CODE( \
	if (!(test)) \
	{ \
		if (AssertImplementation(objID, __FILE_STRIPPED__, __LINE__, -1, StringTraits::AsConstTChars(msg))) \
		{ \
			NON_SOURCE_RELEASE_ONLY(DEBUG_BREAK); \
		} \
	})

#define Assert(test) SOURCE__ASSERT_IMPL(test, InstanceID_None, "Assertion failed on expression: '" #test "'")

#define DebugAssert(x) Assert(x)