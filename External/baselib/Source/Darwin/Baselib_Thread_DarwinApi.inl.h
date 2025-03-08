#pragma once

#include <dlfcn.h>
#include <cstring>
#include <pthread.h>

#ifndef PTHREAD_MAX_NAMELEN_NP
#define PTHREAD_MAX_NAMELEN_NP 64
#endif

namespace DarwinApi
{
    static inline void Thread_SetNameForCurrentThread(const char* name)
    {
        typedef int(*pthread_setname_np_type)(const char*);
        pthread_setname_np_type dynamic_pthread_setname_np;
        dynamic_pthread_setname_np = reinterpret_cast<pthread_setname_np_type>(dlsym(RTLD_DEFAULT, "pthread_setname_np"));
        if (dynamic_pthread_setname_np)
        {
            char name_buffer[PTHREAD_MAX_NAMELEN_NP] = {};
            strncpy(name_buffer, name, sizeof(name_buffer) - 1);
            dynamic_pthread_setname_np(name_buffer);
        }
    }
}
