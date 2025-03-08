#pragma once

#include "Include/C/Baselib_FileIO.h"
#include "Source/ArgumentValidator.h"

static inline bool IsValid(Baselib_FileIO_SyncFile file)
{
    return file.handle != Baselib_FileIO_SyncFile_Invalid.handle;
}

struct AsSyncFileOpenFlags
{
    const Baselib_FileIO_OpenFlags flags;
    AsSyncFileOpenFlags(const Baselib_FileIO_OpenFlags& setFlags)
        : flags(setFlags)
    {
    }
};

static inline bool IsValid(const AsSyncFileOpenFlags& wrapper)
{
    const auto flags = wrapper.flags;
    // check that either read or write is passed
    if ((!(flags & Baselib_FileIO_OpenFlags_Read)) && (!(flags & Baselib_FileIO_OpenFlags_Write)))
        return false;
    // check for conflicting flags
    if ((flags & Baselib_FileIO_OpenFlags_OpenAlways) && (flags & Baselib_FileIO_OpenFlags_CreateAlways))
        return false;
    return true;
}

BASELIB_C_INTERFACE
{
    Baselib_FileIO_EventQueue Baselib_FileIO_EventQueue_Create(void)
    {
        return platform::Baselib_FileIO_EventQueue_Create();
    }

    void Baselib_FileIO_EventQueue_Free(Baselib_FileIO_EventQueue eq)
    {
        if (eq.handle == Baselib_FileIO_EventQueue_Invalid.handle)
            return;
        return platform::Baselib_FileIO_EventQueue_Free(eq);
    }

    uint64_t Baselib_FileIO_EventQueue_Dequeue(
        Baselib_FileIO_EventQueue        eq,
        Baselib_FileIO_EventQueue_Result results[],
        uint64_t                         count,
        uint32_t                         timeoutInMilliseconds
    )
    {
        if (eq.handle == Baselib_FileIO_EventQueue_Invalid.handle)
            return 0;
        if (results == nullptr)
            return 0;
        if (count == 0)
            return 0;
        return platform::Baselib_FileIO_EventQueue_Dequeue(eq, results, count, timeoutInMilliseconds);
    }

    void Baselib_FileIO_EventQueue_Shutdown(Baselib_FileIO_EventQueue eq, uint32_t threadCount)
    {
        platform::Baselib_FileIO_EventQueue_Shutdown(eq, threadCount);
    }

    Baselib_FileIO_AsyncFile Baselib_FileIO_AsyncOpen(
        Baselib_FileIO_EventQueue eq,
        const char*               pathname,
        uint64_t                  userdata,
        Baselib_FileIO_Priority   priority
    )
    {
        if (eq.handle == Baselib_FileIO_EventQueue_Invalid.handle)
            return Baselib_FileIO_AsyncFile_Invalid;
        if (pathname == nullptr)
            return Baselib_FileIO_AsyncFile_Invalid;
        return platform::Baselib_FileIO_AsyncOpen(eq, pathname, userdata, priority);
    }

    void Baselib_FileIO_AsyncRead(
        Baselib_FileIO_AsyncFile   file,
        Baselib_FileIO_ReadRequest requests[],
        uint64_t                   count,
        uint64_t                   userdata,
        Baselib_FileIO_Priority    priority
    )
    {
        if (file.handle == Baselib_FileIO_AsyncFile_Invalid.handle)
            return;
        if (requests == nullptr)
            return;
        if (count == 0)
            return;
        return platform::Baselib_FileIO_AsyncRead(file, requests, count, userdata, priority);
    }

    void Baselib_FileIO_AsyncClose(
        Baselib_FileIO_AsyncFile file
    )
    {
        if (file.handle == Baselib_FileIO_AsyncFile_Invalid.handle)
            return;
        return platform::Baselib_FileIO_AsyncClose(file);
    }

    Baselib_FileIO_SyncFile Baselib_FileIO_SyncOpen(
        const char*                           pathname,
        Baselib_FileIO_OpenFlags              openFlags,
        Baselib_ErrorState*                   errorState
    )
    {
        errorState |= Validate(AsPointer(pathname));
        errorState |= Validate(AsSyncFileOpenFlags(openFlags));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_FileIO_SyncFile_Invalid;

        return platform::Baselib_FileIO_SyncOpen(pathname,
            openFlags,
            errorState
        );
    }

    Baselib_FileIO_SyncFile Baselib_FileIO_SyncFileFromNativeHandle(
        uint64_t handle,
        uint32_t type
    )
    {
        return platform::Baselib_FileIO_SyncFileFromNativeHandle(
            handle,
            type
        );
    }

    uint64_t Baselib_FileIO_SyncRead(
        Baselib_FileIO_SyncFile      file,
        uint64_t                     offset,
        void*                        buffer,
        uint64_t                     size,
        Baselib_ErrorState*          errorState
    )
    {
        errorState |= Validate(file);
        errorState |= Validate(AsPointer(buffer));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return 0;

        uint64_t transferred = 0;
        do
        {
            bool shouldContinue = false;
            const auto result = platform::Baselib_FileIO_SyncRead(
                file,
                offset + transferred,
                reinterpret_cast<uint8_t*>(buffer) + transferred,
                size - transferred,
                shouldContinue,
                errorState
            );
            transferred += result;
            if (!shouldContinue)
                break;
        }
        while (transferred < size);

        return transferred;
    }

    uint64_t Baselib_FileIO_SyncWrite(
        Baselib_FileIO_SyncFile      file,
        uint64_t                     offset,
        const void*                  buffer,
        uint64_t                     size,
        Baselib_ErrorState*          errorState
    )
    {
        errorState |= Validate(file);
        errorState |= Validate(AsPointer(buffer));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return 0;

        uint64_t transferred = 0;
        do
        {
            bool shouldContinue = false;
            const auto result = platform::Baselib_FileIO_SyncWrite(
                file,
                offset + transferred,
                reinterpret_cast<const uint8_t*>(buffer) + transferred,
                size - transferred,
                shouldContinue,
                errorState
            );
            transferred += result;
            if (!shouldContinue)
                break;
        }
        while (transferred < size);

        return transferred;
    }

    void Baselib_FileIO_SyncFlush(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        errorState |= Validate(file);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        return platform::Baselib_FileIO_SyncFlush(file, errorState);
    }

    void Baselib_FileIO_SyncSetFileSize(
        Baselib_FileIO_SyncFile file,
        uint64_t                size,
        Baselib_ErrorState*     errorState
    )
    {
        errorState |= Validate(file);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        return platform::Baselib_FileIO_SyncSetFileSize(file, size, errorState);
    }

    uint64_t Baselib_FileIO_SyncGetFileSize(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        errorState |= Validate(file);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return 0;

        return platform::Baselib_FileIO_SyncGetFileSize(file, errorState);
    }

    void Baselib_FileIO_SyncClose(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        errorState |= Validate(file);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        return platform::Baselib_FileIO_SyncClose(file, errorState);
    }
}
