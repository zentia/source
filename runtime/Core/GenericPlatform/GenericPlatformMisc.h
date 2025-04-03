#pragma once

#include "SourcePrefix.h"

namespace AppReturnType
{
	enum Type : std::uint8_t
	{
		No,
		Yes,
		YesAll,
		NoAll,
		Cancel,
		Ok,
		Retry,
		Continue,
	};
}

namespace AppMsgType
{
	enum Type : std::uint8_t
	{
		Ok,
		YesNo,
		OkCancel,
		YesNoCancel,
		CancelRetryContinue,
		YesNoYesAllNoAll,
		YesNoYesAllNoAllCancel,
		YesNoYesAll,
	};
}

struct GenericPlatformMisc
{
	
};
