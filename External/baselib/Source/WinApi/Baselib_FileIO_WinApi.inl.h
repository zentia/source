#pragma once

#include "Include/C/Baselib_FileIO.h"
#include "Source/Baselib_InlineImplementation.h"
#include "Source/Common/Baselib_FileIO_Common.inl.h"
#include "Source/StringConversions.h"
#include "Source/WinApi/ErrorStateBuilder_WinApi.inl.h"

#include <windows.h>
#include <limits>

namespace WinApi
{
namespace detail
{
    static const uint64_t Baselib_FileIO_MaxOperationSize = std::numeric_limits<DWORD>::max();
}

    BASELIB_INLINE_IMPL Baselib_FileIO_SyncFile Baselib_FileIO_SyncOpen(
        const char*                           pathname,
        Baselib_FileIO_OpenFlags              openFlags,
        Baselib_ErrorState*                   errorState
    )
    {
        ::detail::string_container_128byte_utf16 stringContainer;
        const WCHAR* pathnameW = reinterpret_cast<const WCHAR*>(::detail::StrUtf8ToUtf16(pathname, stringContainer));

        const DWORD fileAttributes = FILE_ATTRIBUTE_NORMAL;

        const DWORD fileFlags = 0;

        DWORD desiredAccess = 0;
        if (openFlags & Baselib_FileIO_OpenFlags_Read)
            desiredAccess |= GENERIC_READ;
        if (openFlags & Baselib_FileIO_OpenFlags_Write) // absence of "else" is intentional
            desiredAccess |= GENERIC_WRITE;

        const DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

        DWORD creationDisposition = 0;
        if (openFlags & Baselib_FileIO_OpenFlags_OpenAlways)
            creationDisposition = OPEN_ALWAYS;
        else if (openFlags & Baselib_FileIO_OpenFlags_CreateAlways)
            creationDisposition = CREATE_ALWAYS;
        else
            creationDisposition = OPEN_EXISTING;

        #if BASELIB_PLATFORM_WINDOWS // CreateFile2 not available on Win7
        HANDLE h = CreateFileW(
            pathnameW,
            desiredAccess,
            shareMode,
            NULL,
            creationDisposition,
            fileAttributes | fileFlags,
            NULL
        );
        #else // CreateFileW not available on UWP
        CREATEFILE2_EXTENDED_PARAMETERS params;
        memset(&params, 0, sizeof(params));
        params.dwSize = sizeof(params);
        params.dwFileAttributes = fileAttributes;
        params.dwFileFlags = fileFlags;
        HANDLE h = CreateFile2(
            pathnameW,
            desiredAccess,
            shareMode,
            creationDisposition,
            &params
        );
        #endif

        if (h == INVALID_HANDLE_VALUE)
        {
            Baselib_ErrorCode errorCode = Baselib_ErrorCode_IOError;
            const auto lastError = GetLastError();
            switch (lastError)
            {
                case ERROR_FILE_NOT_FOUND:
                    errorCode = Baselib_ErrorCode_InvalidPathname;
                    break;
                case ERROR_ACCESS_DENIED:
                {
                    const auto fileAttributes = GetFileAttributesW(pathnameW);
                    if ((fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        errorCode = Baselib_ErrorCode_InvalidPathname;
                    else
                        errorCode = Baselib_ErrorCode_RequestedAccessIsNotAllowed;
                    break;
                }
                default:
                    break;
            }
            errorState |= RaiseError(errorCode) | WithGetLastError(lastError);
            return Baselib_FileIO_SyncFile_Invalid;
        }

        return ::detail::AsBaselibHandle<Baselib_FileIO_SyncFile>(h);
    }

    BASELIB_INLINE_IMPL Baselib_FileIO_SyncFile Baselib_FileIO_SyncFileFromNativeHandle(uint64_t handle, uint32_t type)
    {
        switch (static_cast<Baselib_FileIO_NativeHandleType>(type))
        {
            case Baselib_FileIO_NativeHandleType_WinApiHandle:
            {
                auto h = reinterpret_cast<HANDLE>(handle);
                if (h == INVALID_HANDLE_VALUE)
                    return Baselib_FileIO_SyncFile_Invalid;
                return ::detail::AsBaselibHandle<Baselib_FileIO_SyncFile>(h);
            }
        }

        return Baselib_FileIO_SyncFile_Invalid;
    }

    BASELIB_INLINE_IMPL uint64_t Baselib_FileIO_SyncRead(
        Baselib_FileIO_SyncFile file,
        uint64_t                offset,
        void*                   buffer,
        uint64_t                size,
        bool&                   shouldContinue,
        Baselib_ErrorState*     errorState
    )
    {
        auto h = ::detail::AsNativeType<HANDLE>(file);

        LARGE_INTEGER offsetLI = {};
        offsetLI.QuadPart = offset;

        OVERLAPPED overlapped = {};
        overlapped.Offset = offsetLI.LowPart;
        overlapped.OffsetHigh = offsetLI.HighPart;

        DWORD bytes = 0;
        if (ReadFile(
            h,
            reinterpret_cast<void*>(buffer),
            static_cast<DWORD>(std::min(size, detail::Baselib_FileIO_MaxOperationSize)),
            &bytes,
            &overlapped
            ) == FALSE)
        {
            shouldContinue = false;

            const auto lastError = GetLastError();

            // Windows reports an error if we read pass EOF, but amount of bytes is valid
            if (lastError == ERROR_HANDLE_EOF)
                return bytes;
            else
            {
                errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
                // Current bytes value might be invalid, so we ignore it and report known good bytes transferred.
                return 0;
            }
        }
        else
        {
            shouldContinue = bytes > 0;
            return bytes;
        }
    }

    BASELIB_INLINE_IMPL uint64_t Baselib_FileIO_SyncWrite(
        Baselib_FileIO_SyncFile file,
        uint64_t                offset,
        const void*             buffer,
        uint64_t                size,
        bool&                   shouldContinue,
        Baselib_ErrorState*     errorState
    )
    {
        // Zero write has a special meaning in WinApi, which why it stands to reason that we should not skip it.
        // From https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
        //
        // > A value of zero specifies a null write operation.
        // > The behavior of a null write operation depends on the underlying file system or communications technology.
        // [...]
        // > The system interprets zero bytes to write as specifying a null write operation and WriteFile does not truncate or extend the file.
        //
        // However, we observed spurious failure via ERROR_NO_SYSTEM_RESOURCES (1450) for zero sized writes on some WinApi platforms.
        // (see https://github.cds.internal.unity3d.com/unity/baselib/issues/629)
        // As of writing, the skip on zero size behavior is consistent with the pre-existing Unity implementation:
        // https://github.cds.internal.unity3d.com/unity/unity/blob/2135fd4aa57c5ceb0f4495fb0afd4086025c1d85/PlatformDependent/Win/LocalFileSystemWindowsShared.cpp#L702
        //
        // TODO: Revisit decision to pass on zero writes in general.
        if (size == 0)
        {
            shouldContinue = false;
            return 0;
        }

        auto h = ::detail::AsNativeType<HANDLE>(file);

        LARGE_INTEGER offsetLI = {};
        offsetLI.QuadPart = offset;

        OVERLAPPED overlapped = {};
        overlapped.Offset = offsetLI.LowPart;
        overlapped.OffsetHigh = offsetLI.HighPart;

        DWORD bytes = 0;
        if (WriteFile(
            h,
            reinterpret_cast<const void*>(buffer),
            static_cast<DWORD>(std::min(size, detail::Baselib_FileIO_MaxOperationSize)),
            &bytes,
            &overlapped
            ) == FALSE)
        {
            shouldContinue = false;

            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
            // Current bytes value might be invalid, so we ignore it and report known good bytes transferred.
            return 0;
        }
        else
        {
            shouldContinue = bytes > 0;
            return bytes;
        }
    }

    BASELIB_INLINE_IMPL void Baselib_FileIO_SyncFlush(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        auto h = ::detail::AsNativeType<HANDLE>(file);

        if (FlushFileBuffers(h) == FALSE)
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
        }
    }

    void Baselib_FileIO_SyncSetFileSize(
        Baselib_FileIO_SyncFile file,
        uint64_t                size,
        Baselib_ErrorState*     errorState
    )
    {
        auto h = ::detail::AsNativeType<HANDLE>(file);

        LARGE_INTEGER largeInteger = {};
        largeInteger.QuadPart = size;

        if (SetFilePointerEx(h, largeInteger, NULL, FILE_BEGIN) == FALSE)
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
            return;
        }

        if (SetEndOfFile(h) == FALSE)
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
            return;
        }
    }

    BASELIB_INLINE_IMPL uint64_t Baselib_FileIO_SyncGetFileSize(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        auto h = ::detail::AsNativeType<HANDLE>(file);

        LARGE_INTEGER size = {};
        if (GetFileSizeEx(h, &size) == 0)
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
            return 0;
        }
        return size.QuadPart;
    }

    BASELIB_INLINE_IMPL void Baselib_FileIO_SyncClose(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        auto h = ::detail::AsNativeType<HANDLE>(file);

        if (CloseHandle(h) == FALSE)
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithGetLastError(lastError);
        }
    }
}
