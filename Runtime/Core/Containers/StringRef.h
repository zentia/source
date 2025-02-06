#pragma once
#include "Runtime/Core/Containers/StringOperations.h"

namespace core
{
    template<typename TChar>
    class basic_string_ref : private basic_string_operations<TChar>
    {
        
    };
}