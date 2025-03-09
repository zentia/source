#pragma once

#include "FormatArgs.h"
#include "Runtime/Core/Containers/String.h"
#include "Runtime/Core/Containers/StringRef.h"

namespace core
{
	namespace detail
	{
		core::string FormatInternal(string_ref formatStr, const FormatArgs& args);
		void FormatToInternal(string& output, core::string_ref formatStr, const FormatArgs& args);
	}

	template<class ... Args>
	core::string Format(string_ref formatStr, const Args& ... args)
	{
		FormatArgsN<sizeof ...(Args)> formatArgs_wrapper;
		const FormatArgs& formatArgs = *reinterpret_cast<const FormatArgs*>(&formatArgs_wrapper.argInfo);
		return detail::FormatInternal(formatStr, formatArgs);
	}
}