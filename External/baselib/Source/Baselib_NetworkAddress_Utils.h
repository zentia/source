#pragma once

#include "Include/Baselib.h"
#include "Include/C/Baselib_NetworkAddress.h"
#include "Source/Baselib_ErrorState_Utils.h"

#include "Source/ArgumentValidator.h"

static inline bool IsValid(Baselib_NetworkAddress_Family family)
{
    switch (family)
    {
        case Baselib_NetworkAddress_Family_IPv4:
        case Baselib_NetworkAddress_Family_IPv6:
            return true;
        default:
            return false;
    }
}
