#pragma once
#include "Runtime/BaseCore/GenericPlatform.h"

typedef GenericPlatformTypes::uint8 uint8;
typedef GenericPlatformTypes::int32 int32;
typedef GenericPlatformTypes::TCHAR TCHAR;

#define WIDETEXT_PASTE(x) L ## x
#define WIDETEXT(str) WIDETEXT_PASTE(str)
#define TEXT_PASTE(x) WIDETEXT(x)
#define TEXT(x) TEXT_PASTE(x)