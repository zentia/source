#pragma once

#include "Include/Baselib.h"
#include "Include/Cpp/Lock.h"
#include "Source/Posix/Baselib_DynamicLibrary_PosixApi.inl.h"
#include "Source/STLMemoryUtils.h"

#include <string>
#include <unordered_map>

namespace QNXApi
{
namespace detail
{
    // Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
    BASELIB_CPP_INTERFACE
    {
        class DynamicLibrariesContext
        {
        private:
            struct DynamicLibrary
            {
                size_t refCount = 0;
                Baselib_DynamicLibrary_Handle handle = Baselib_DynamicLibrary_Handle_Invalid;

                DynamicLibrary(size_t r, Baselib_DynamicLibrary_Handle& h) :
                    refCount(r),
                    handle(h)
                {}
            };

            template<typename K, typename V>
            using DynamicLibraryCacheMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, baselib::STLAllocator<std::pair<const K, V> > >;
            using DynamicLibraryCache = DynamicLibraryCacheMap<std::string, DynamicLibrary>;

            baselib::Lock lock;
            DynamicLibraryCache cache;

        public:
            static DynamicLibrariesContext& Get()
            {
                static DynamicLibrariesContext context;
                return context;
            }

            Baselib_DynamicLibrary_Handle OpenLibrary(
                const char* pathnameUtf8,
                Baselib_ErrorState* errorState
            )
            {
                BaselibAssert(errorState);

                auto lockScope = lock.AcquireScoped();

                auto entry = cache.find(pathnameUtf8);
                if (entry != cache.end())
                {
                    ++entry->second.refCount;
                    return entry->second.handle;
                }

                Baselib_DynamicLibrary_Handle handle = PosixApi::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8, errorState);
                if (Baselib_ErrorState_ErrorRaised(errorState))
                {
                    return Baselib_DynamicLibrary_Handle_Invalid;
                }

                cache.emplace(pathnameUtf8, DynamicLibrary(1, handle));

                return handle;
            }

            void CloseLibrary(Baselib_DynamicLibrary_Handle& handle)
            {
                auto lockScope = lock.AcquireScoped();

                for (auto iter = cache.begin(); iter != cache.end(); ++iter)
                {
                    if (iter->second.handle == handle)
                    {
                        if (--iter->second.refCount == 0)
                        {
                            PosixApi::Baselib_DynamicLibrary_Close(handle);
                            cache.erase(iter);
                        }

                        return;
                    }
                }
            }
        };
    }
}

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf8(
        const char* pathnameUtf8, // can be nullptr
        Baselib_ErrorState* errorState
    )
    {
        auto& context = detail::DynamicLibrariesContext::Get();
        return context.OpenLibrary(pathnameUtf8, errorState);
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf16(
        const baselib_char16_t* pathnameUtf16,
        Baselib_ErrorState* errorState
    )
    {
        ::detail::string_container_128byte_utf8 stringContainer;
        const auto pathnameUtf8 = ::detail::StrUtf16ToUtf8(pathnameUtf16, stringContainer);
        return QNXApi::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8, errorState);
    }

    BASELIB_INLINE_IMPL void Baselib_DynamicLibrary_Close(
        Baselib_DynamicLibrary_Handle handle
    )
    {
        auto& context = detail::DynamicLibrariesContext::Get();
        context.CloseLibrary(handle);
    }
}
