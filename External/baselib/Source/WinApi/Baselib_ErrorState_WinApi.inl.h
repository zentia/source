#pragma once

#include "Include/Baselib.h"
#include "Source/Baselib_ErrorState_Utils.h"
#include "Source/omemstream.h"
#include "Source/StringConversions.h"
#include <windows.h>

namespace WinApi
{
    BASELIB_INLINE_IMPL void Baselib_ErrorState_Explain_GetLastError(uint64_t nativeErrorCode, omemstream& out, Baselib_ErrorState_ExplainVerbosity verbosity)
    {
        DWORD errorCode = (DWORD)nativeErrorCode;

        switch (verbosity)
        {
            case Baselib_ErrorState_ExplainVerbosity_ErrorType:
                break;

            case Baselib_ErrorState_ExplainVerbosity_ErrorType_SourceLocation_Explanation:
            {
                WCHAR errorMessage[1024];
                if (FormatMessageW(
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS |
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPWSTR)&errorMessage,
                    sizeof(errorMessage) / sizeof(TCHAR),
                    NULL
                    ) > 0)
                {
                    ::detail::string_container_128byte_utf8 stringContainer;
                    out << " - " << ::detail::StrUtf16ToUtf8(reinterpret_cast<baselib_char16_t*>(errorMessage), stringContainer);
                }
                break;
            }
        }
        out.formatString(" (WinError:%08x)", (int)errorCode);
    }

    BASELIB_INLINE_IMPL uint32_t Baselib_ErrorState_Explain(const Baselib_ErrorState* errorState, char* buffer, uint32_t bufferLen, Baselib_ErrorState_ExplainVerbosity verbosity)
    {
        omemstream out(buffer, bufferLen);
        if (!Baselib_ErrorState_Explain_BaselibErrorCode(errorState, out, verbosity))
            return (uint32_t)out.totalBytes() + 1;

        switch (errorState->nativeErrorCodeType)
        {
            case Baselib_ErrorState_NativeErrorCodeType_GetLastError:
                Baselib_ErrorState_Explain_GetLastError(errorState->nativeErrorCode, out, verbosity);
                break;
        }
        return (uint32_t)out.totalBytes() + 1;
    }
}
