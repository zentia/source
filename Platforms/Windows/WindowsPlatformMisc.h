#pragma once

#include "Runtime/Core/GenericPlatform/GenericPlatformMisc.h"

#define PLATFORM_WIN 1

struct WindowsPlatformMisc : public GenericPlatformMisc
{
	AppReturnType::Type MessageBoxExt(AppMsgType::Type msgType, const wchar_t* text, const wchar_t* caption);
};

typedef WindowsPlatformMisc PlatformMisc;