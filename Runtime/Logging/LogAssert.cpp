#include "SourcePrefix.h"
#include "LogAssert.h"

#include <cstdarg>

static FILE* gConsoleFile = nullptr;

enum class LineEndInfoHint
{
	None,
	EndsWithNewLine,
};

#define LOG_FORCE_INLINE inline

static void EnsureStdOutInitialized()
{
	if (gConsoleFile == nullptr)
		gConsoleFile = stdout;
}

void printf_consolev_to_console_file_or_stdout(const char* log, va_list alist, bool flushOnWrite)
{
	EnsureStdOutInitialized();

	if (gConsoleFile == nullptr)
		return;

	va_list list;
	va_copy(list, alist);
	vfprintf(gConsoleFile, log, list);

	if (flushOnWrite)
		fflush(gConsoleFile);

	va_end(list);
}

static inline void printf_consolev_helper(LogType logType, const char* log, va_list alist, bool flushOnWrite)
{
	printf_consolev_to_console_file_or_stdout(log, alist, flushOnWrite);
}

void printf_consolev(LogType logType, const char* logInput, va_list alist, bool flushOnWrite, LineEndInfoHint lineEndInfoHint)
{
	printf_consolev_helper(logType, logInput, alist, flushOnWrite);
}

static void InternalLogConsoleWithNewLine(const char* log, ...)
{
	va_list vl;
	va_start(vl, log);
	printf_consolev(kLogTypeLog, log, vl, true, LineEndInfoHint::EndsWithNewLine);
	va_end(vl);
}

typedef void PrintConsole(const char* log, ...);

inline LogType LogModeToLogType(int mode)
{
	LogType logType;
	if (mode & kError)
		logType = kLogTypeError;
	else
		logType = kLogTypeLog;
	return logType;
}

void DebugStringToFilePostprocessedStacktrace(const DebugStringToFileData& data)
{
	LogType logType = LogModeToLogType(data.mode);
	PrintConsole* printConsole = InternalLogConsoleWithNewLine;
	core::string message = core::string::create_from_external(data.message, kMemTempAlloc);
	printConsole("%s", message.c_str());
}

void DebugStringToFile(const DebugStringToFileData& data)
{
	LogType logType = LogModeToLogType(data.mode);
}

bool AssertImplementation(InstanceID objID, const char* fileStripped, int line, int column, const char* msg)
{
	DebugStringToFile(msg, fileStripped, line, column, kAssert, objID);
	return true;
}