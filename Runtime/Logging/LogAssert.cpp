#include "SourcePrefix.h"
#include "LogAssert.h"

#include <cstdarg>

#include "Editor/Src/Application/Application.h"
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

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

static inline void printf_consolev_helper(LogSystem::LogLevel logType, const char* log, va_list alist, bool flushOnWrite)
{
	printf_consolev_to_console_file_or_stdout(log, alist, flushOnWrite);
}

void printf_consolev(LogSystem::LogLevel logType, const char* logInput, va_list alist, bool flushOnWrite, LineEndInfoHint lineEndInfoHint)
{
	printf_consolev_helper(logType, logInput, alist, flushOnWrite);
}

static void InternalLogConsoleWithNewLine(const char* log, ...)
{
	va_list vl;
	va_start(vl, log);
	printf_consolev(LogSystem::LogLevel::Debug, log, vl, true, LineEndInfoHint::EndsWithNewLine);
	va_end(vl);
}

typedef void PrintConsole(const char* log, ...);

inline LogSystem::LogLevel LogModeToLogType(int mode)
{
	LogSystem::LogLevel logType;
	if (mode & kError)
		logType = LogSystem::LogLevel::Error;
	else
		logType = LogSystem::LogLevel::Info;
	return logType;
}

void DebugStringToFilePostprocessedStacktrace(const DebugStringToFileData& data)
{
	LogSystem::LogLevel logType = LogModeToLogType(data.mode);
	PrintConsole* printConsole = InternalLogConsoleWithNewLine;
	core::string message = core::string::create_from_external(data.message, kMemTempAlloc);
	printConsole("%s", message.c_str());
}

void DebugStringToFile(const DebugStringToFileData& data)
{
	LogSystem::LogLevel logType = LogModeToLogType(data.mode);
	GetApplication().GetLogSystem()->Log(logType, data.message);
}

bool AssertImplementation(InstanceID objID, const char* fileStripped, int line, int column, const char* msg)
{
	DebugStringToFile(msg, fileStripped, line, column, kAssert, objID);
	return true;
}

LogSystem::LogSystem()
{
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_level(spdlog::level::trace);
	consoleSink->set_pattern("[%^%l%$] %v");

	const spdlog::sinks_init_list sink_list = { consoleSink };

	spdlog::init_thread_pool(8192, 1);

	m_Logger = std::make_shared<spdlog::async_logger>("muggle_logger", sink_list.begin(), sink_list.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

	m_Logger->set_level(spdlog::level::trace);

	spdlog::register_logger(m_Logger);
}

LogSystem::~LogSystem()
{
	m_Logger->flush();
	spdlog::drop_all();
}
