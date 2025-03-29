#pragma once

#include <memory>

#include "Runtime/BaseClasses/InstanceID.h"
#include "Runtime/Modules/ExportModuels.h"
#include "Runtime/Utitlities/StringTraits.h"
#include <spdlog/spdlog.h>

enum LogMessageFlags
{
	kNoLogMessageFlags = 0,
	kError = 1 << 0,
	kAssert = 1 << 1,
	kLog = 1 << 2,
	kDebug = 1 << 3,
	kFatal = 1 << 4,
	kWarning = 1 << 9,
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

#define LOG_DEBUG(...) PP_WRAP_CODE(DebugStringToFile(Format(__VA_ARGS__),__FILE_STRIPPED__,__LINE__,-1,kDebug))
#define LOG_ERROR(...)		PP_WRAP_CODE(DebugStringToFile (__VA_ARGS__, __FILE_STRIPPED__, __LINE__, -1, kError))
#define LOG_ERROR_FORMAT(...)		PP_WRAP_CODE(DebugStringToFile (Format(__VA_ARGS__), __FILE_STRIPPED__, __LINE__, -1, kError))
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

class log_system
{
public:
	enum class LogLevel : std::uint8_t
	{
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};
	log_system();
	~log_system();

	template<typename... TARGS>
	void Log(LogLevel level, TARGS&&... args)
	{
		switch (level)
		{
		case LogLevel::Debug:
			m_logger_->debug(std::forward<TARGS>(args)...);
			break;
		case LogLevel::Info:
			m_logger_->info(std::forward<TARGS>(args)...);
			break;
		case LogLevel::Error:
			m_logger_->error(std::forward<TARGS>(args)...);
			break;
		}
	}
private:
	std::shared_ptr<spdlog::logger> m_logger_;
};
