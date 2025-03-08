#pragma once

#include "Configuration/SourceConfigure.h"
#include "baselib/Include/Cpp/ThreadLocalStorage.h"

#if SOURCE_DYNAMIC_TLS

template<typename T>
class ThreadSpecificValue : public baselib::ThreadLocalStorage<T>
{
public:

};

#define SOURCE_TLS_VALUE(type) ThreadSpecificValue<type>

#endif
