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
	kFatal = 1 << 4,
	kWarning = 1 << 9,
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

void DebugStringToFile(const DebugStringToFileData& data);
core::string Format(const char* format, ...);

template<typename TString>
void DebugStringToFile(const TString& message, const char* file, int line, int column, int mode, const InstanceID objectInstanceID = InstanceID_None, int identifier = 0)
{
	DebugStringToFileData data;
	data.message = StringTraits::AsConstTChars(message);
	data.mode = (LogMessageFlags)mode;
	DebugStringToFile(data);
}

#define ErrorStringMsg(...)		PP_WRAP_CODE(DebugStringToFile (Format(__VA_ARGS__), __FILE_STRIPPED__, __LINE__, -1, kError))
#define WarningStringMsg(...)	PP_WRAP_CODE(DebugStringToFile (Format(__VA_ARGS__), __FILE_STRIPPED__, __LINE__, -1, kWarning))
#define FatalErrorMsg(...)		PP_WRAP_CODE(DebugStringToFile (Format(__VA_ARGS__), __FILE_STRIPPED__, __LINE__, -1, kError | kFatal))

#include "DebugBreak.h"

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
#define AssertMsg(test, msg, ...) SOURCE__ASSERT_IMPL(test, GetInstanceIDFrom(__VA_ARGS__), msg)

#define DebugAssert(x) Assert(x)