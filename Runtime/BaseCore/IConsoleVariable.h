#pragma once

#include "Runtime/BaseCore/IConsoleObject.h"
#include "CoreTypes.h"

class IConsoleVariable : public IConsoleObject
{
public:
    virtual void Set(const TCHAR* value);

    virtual bool GetBool() const = 0;
    virtual int32 GetInt() const = 0;
    virtual float GetFloat() const = 0;
    
};