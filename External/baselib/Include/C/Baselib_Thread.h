#pragma once

#include "Baselib_Timer.h"
#include "Baselib_ErrorState.h"

BASELIB_C_INTERFACE
{

typedef intptr_t Baselib_Thread_Id;

static const Baselib_Thread_Id Baselib_Thread_InvalidId = 0;

static const size_t Baselib_Thread_MaxThreadNameLength = 64;

BASELIB_API void Baselib_Thread_YieldExecution(void);

BASELIB_API Baselib_Thread_Id Baselib_Thread_GetCurrentThreadId(void);

static const int Baselib_Thread_MinGuaranteedMaxConcurrentThreads = 64;

typedef struct Baselib_Thread Baselib_Thread;

typedef void (*Baselib_Thread_EntryPointFunction)(void* arg);

typedef struct Baselib_Thread_Config
{
    const char* name;

    uint64_t stackSize;

    Baselib_Thread_EntryPointFunction entryPoint;

    void* entryPointArgument;
} Baselib_Thread_Config;

BASELIB_API Baselib_Thread* Baselib_Thread_Create(Baselib_Thread_Config config, Baselib_ErrorState* errorState);

BASELIB_API void Baselib_Thread_Join(Baselib_Thread* thread, uint32_t timeoutInMilliseconds, Baselib_ErrorState* errorState);

BASELIB_API Baselib_Thread_Id Baselib_Thread_GetId(Baselib_Thread* thread);

BASELIB_API bool Baselib_Thread_SupportsThreads(void);

} // BASELIB_C_INTERFACE
