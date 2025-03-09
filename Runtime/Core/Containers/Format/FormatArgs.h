#pragma once

#include "Runtime/Core/Containers/String.h"
#include "Runtime/Core/Containers/StringRef.h"
#include "Runtime/Utitlities/NonCopyable.h"

typedef const void* FormatArgValue;

typedef void(*FormatToFuncPtr)(core::string&, core::string_ref, const FormatArgValue*);

struct FormatArgTypeInfo
{
	FormatToFuncPtr formatter;
};


struct NamedFormatArg
{
	FormatArgValue valuePtr;
	FormatArgTypeInfo typeInfo;
	const char* name;

	template<typename T> FORCE_INLINE NamedFormatArg(const T& value);
	template<typename T> FORCE_INLINE NamedFormatArg(const char* name, const T& value);
};
struct FormatArgInfo
{
	SInt32 count;
	UInt32 namedArgFlags;
	const FormatArgTypeInfo* GetArgTypeInfoPtr() const
	{
		return reinterpret_cast<const FormatArgTypeInfo*>(this + 1);
	}
};

template<int SIZE>
struct FormatArgInfoN
{
	FormatArgInfo info;
	FormatArgTypeInfo a[SIZE];
};

template<int ARGCOUNT>
struct FormatArgsN
{
	const FormatArgInfo* argInfo;
	const void* argPointers[ARGCOUNT];
};

class FormatArgs : NonCopyable
{
public:
private:
	const FormatArgInfo* argInfo;
};

template<class T, bool PassAsValue>
struct FormatArgMapTypeHelperBase
{
	static void FormatArgToVoidPtr(const void** pp, const T& t)
	{
		*pp = &t;
	}

	static void FormatArgFormatter(core::string& outputBuf, core::string_ref formatExpr, const FormatArgValue* valuePtr);
};