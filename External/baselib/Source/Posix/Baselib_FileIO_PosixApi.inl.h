#pragma once

#include "Include/C/Baselib_FileIO.h"
#include "Source/Baselib_Utilities.h"
#include "Source/Baselib_InlineImplementation.h"
#include "Source/Common/Baselib_FileIO_Common.inl.h"
#include "Source/Posix/ErrorStateBuilder_PosixApi.inl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>

namespace PosixApi
{
namespace detail
{
    // https://www.man7.org/linux/man-pages/man2/read.2.html
    // On Linux, read() (and similar system calls) will transfer at most
    // 0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
    // actually transferred.  (This is true on both 32-bit and 64-bit
    // systems.)
    // On some BSD system the limit is defined to be INT_MAX.
    // In POSIX it's defined to be SSIZE_MAX.
    // So we are taking the min of these three here.
    static const uint64_t Baselib_FileIO_MaxOperationSize = 0x7ffff000;

    template<typename T>
    static inline int TryAgainOnEINTR(T callback)
    {
        while (true)
        {
            int result = callback();
            if ((result == -1) && (errno == EINTR))
                continue;
            else
                return result;
        }
    }
}

#define BASELIB_TRY_AGAIN_ON_EINTR(__expr) detail::TryAgainOnEINTR([&]()->int{return (__expr);})

    BASELIB_INLINE_IMPL Baselib_FileIO_SyncFile Baselib_FileIO_SyncOpen(
        const char*                           pathname,
        Baselib_FileIO_OpenFlags              openFlags,
        Baselib_ErrorState*                   errorState
    )
    {
        int flags = 0;

        // On Posix O_RDWR != O_RDONLY | O_WRONLY;
        if ((openFlags & Baselib_FileIO_OpenFlags_Read) && (openFlags & Baselib_FileIO_OpenFlags_Write))
            flags |= O_RDWR;
        else if (openFlags & Baselib_FileIO_OpenFlags_Read)
            flags |= O_RDONLY;
        else if (openFlags & Baselib_FileIO_OpenFlags_Write)
            flags |= O_WRONLY;

        if (openFlags & Baselib_FileIO_OpenFlags_OpenAlways)
            flags |= O_CREAT;
        else if (openFlags & Baselib_FileIO_OpenFlags_CreateAlways)
            flags |= O_CREAT | O_TRUNC;

        const mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;

        const int fd = BASELIB_TRY_AGAIN_ON_EINTR(open(pathname, flags, mode));
        if (fd == -1)
        {
            const int error = errno;
            if (error == ENOENT)
                errorState |= RaiseError(Baselib_ErrorCode_InvalidPathname) | WithErrno(error);
            else if (error == EISDIR)
                errorState |= RaiseError(Baselib_ErrorCode_InvalidPathname) | WithErrno(error) | WithStaticString("The pathname points to a directory");
            else if (error == EACCES || error == EPERM)
                errorState |= RaiseError(Baselib_ErrorCode_RequestedAccessIsNotAllowed) | WithErrno(error);
            else
                errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
            return Baselib_FileIO_SyncFile_Invalid;
        }

        struct stat statBuf;
        if (fstat(fd, &statBuf) == 0) // does not return EINTR
        {
            if (S_ISDIR(statBuf.st_mode))
            {
                errorState |= RaiseError(Baselib_ErrorCode_InvalidPathname) | WithStaticString("The pathname points to a directory");
                BASELIB_TRY_AGAIN_ON_EINTR(close(fd)); // error is ignored
                return Baselib_FileIO_SyncFile_Invalid;
            }
        }
        else
        {
            const int error = errno;
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
            BASELIB_TRY_AGAIN_ON_EINTR(close(fd)); // error is ignored
            return Baselib_FileIO_SyncFile_Invalid;
        }

        return ::detail::AsBaselibHandle<Baselib_FileIO_SyncFile>(fd);
    }

    BASELIB_INLINE_IMPL Baselib_FileIO_SyncFile Baselib_FileIO_SyncFileFromNativeHandle(uint64_t handle, uint32_t type)
    {
        switch (static_cast<Baselib_FileIO_NativeHandleType>(type))
        {
            case Baselib_FileIO_NativeHandleType_PosixFd:
            {
                const int fd = static_cast<int>(handle);
                if (fd == -1)
                    return Baselib_FileIO_SyncFile_Invalid;
                return ::detail::AsBaselibHandle<Baselib_FileIO_SyncFile>(fd);
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
        const int fd = ::detail::AsNativeType<int>(file);

        const ssize_t bytes = pread(
            fd,
            buffer,
            static_cast<uint32_t>(std::min(size, detail::Baselib_FileIO_MaxOperationSize)),
            static_cast<off_t>(offset)
        );

        if (bytes == -1)
        {
            const int error = errno;
            if (error == EINTR)
                shouldContinue = true; // ignore EINTR and continue even if we transferred 0 bytes
            else
            {
                shouldContinue = false;
                errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
            }
            return 0;
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
        const int fd = ::detail::AsNativeType<int>(file);

        const ssize_t bytes = pwrite(
            fd,
            buffer,
            static_cast<uint32_t>(std::min(size, detail::Baselib_FileIO_MaxOperationSize)),
            static_cast<off_t>(offset)
        );

        if (bytes == -1)
        {
            const int error = errno;
            if (error == EINTR)
                shouldContinue = true; // ignore EINTR and continue even if we transferred 0 bytes
            else
            {
                shouldContinue = false;
                errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
            }
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
        const int fd = ::detail::AsNativeType<int>(file);
        if (fsync(fd) == -1) // does not return EINTR
        {
            const int error = errno;
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
        }
    }

    BASELIB_INLINE_IMPL void Baselib_FileIO_SyncSetFileSize(
        Baselib_FileIO_SyncFile file,
        uint64_t                size,
        Baselib_ErrorState*     errorState
    )
    {
        const int fd = ::detail::AsNativeType<int>(file);

        if (BASELIB_TRY_AGAIN_ON_EINTR(ftruncate(fd, size)) == -1)
        {
            const int error = errno;
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
        }
    }

    BASELIB_INLINE_IMPL uint64_t Baselib_FileIO_SyncGetFileSize(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        const int fd = ::detail::AsNativeType<int>(file);

        struct stat statBuf;
        if (fstat(fd, &statBuf) == -1)
        {
            const int error = errno;
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
            return 0;
        }
        return statBuf.st_size;
    }

    BASELIB_INLINE_IMPL void Baselib_FileIO_SyncClose(
        Baselib_FileIO_SyncFile file,
        Baselib_ErrorState*     errorState
    )
    {
        const int fd = ::detail::AsNativeType<int>(file);
        if (BASELIB_TRY_AGAIN_ON_EINTR(close(fd)) == -1)
        {
            const int error = errno;
            errorState |= RaiseError(Baselib_ErrorCode_IOError) | WithErrno(error);
        }
    }

#undef BASELIB_TRY_AGAIN_ON_EINTR
}
