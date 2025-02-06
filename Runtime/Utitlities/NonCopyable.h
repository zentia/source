#pragma once

#include "Runtime/Modules/ExportModuels.h"

class EXPORT_COREMODULE NonCopyable
{
public:
    constexpr NonCopyable() {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};