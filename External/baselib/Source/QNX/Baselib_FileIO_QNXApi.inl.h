#pragma once

#include "Source/Posix/Baselib_FileIO_PosixApi.inl.h"

namespace QNXApi
{
    BASELIB_INLINE_IMPL Baselib_FileIO_SyncFile Baselib_FileIO_SyncOpen(
        const char*                           pathname,
        Baselib_FileIO_OpenFlags              openFlags,
        Baselib_ErrorState*                   errorState
    )
    {
        if (Baselib_FileIO_OpenFlags_CreateAlways == (openFlags & Baselib_FileIO_OpenFlags_CreateAlways))
        {
            // Since "create always" translate to O_CREAT | O_TRUNC on POSIX and on QNX, opening the file as
            // read-only when O_TRUNC is specified has no effect (http://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.lib_ref/topic/o/open.html)
            // we have to ensure that Baselib_FileIO_OpenFlags_Write is implicitly added as the
            // documentation for Baselib_FileIO_OpenFlags_CreateAlways specifies is possible for certain platforms.
            openFlags |= Baselib_FileIO_OpenFlags_Write;
        }
        return PosixApi::Baselib_FileIO_SyncOpen(pathname, openFlags, errorState);
    }
}
