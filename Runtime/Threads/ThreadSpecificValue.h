#pragma once

#include "Configuration/SourceConfigure.h"

#if SOURCE_DYNAMIC_TLS

template<typename T>
class ThreadSpecificValue
{
public:

};

#define SOURCE_TLS_VALUE(type) ThreadSpecificValue<type>

#endif
