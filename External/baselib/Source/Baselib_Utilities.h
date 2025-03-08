#pragma once

#include <string.h>

namespace detail
{
    template<typename NativeType, typename BaselibHandleType>
    static inline NativeType AsNativeType(BaselibHandleType baselibHandle)
    {
        static_assert(sizeof(BaselibHandleType::handle) >= sizeof(NativeType), "native type does not fit baselib handle");
        NativeType result = {};
        memcpy(&result, &baselibHandle.handle, sizeof(result));
        return result;
    }

    template<typename BaselibHandleType, typename NativeType>
    static inline BaselibHandleType AsBaselibHandle(NativeType nativeType)
    {
        static_assert(sizeof(BaselibHandleType::handle) >= sizeof(NativeType), "native type does not fit baselib handle");
        decltype(BaselibHandleType::handle)result = {};
        memcpy(&result, &nativeType, sizeof(nativeType));
        return BaselibHandleType { result };
    }

    static inline size_t StrLenUtf16(const baselib_char16_t* utf16String)
    {
        size_t length = 0;
        while (*utf16String)
        {
            utf16String++;
            length++;
        }
        return length;
    }
}
