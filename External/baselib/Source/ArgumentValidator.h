#pragma once

// Argument validator based on error state builder.
//
// For strong type arguments, implement IsValid templated function, return true.
// Because some arguments types are weakly typed, we use helper wrappers to make them strong typed.
//
// Usage examples:
// errorState |= Validate(strongTypedArgument);
// errorState |= Validate(AsPointer(weaklyTypedPointer));
// errorState |= Validate(strongTypedArgument) | WithStaticString("extra explanation if validate raises an error");
// errorState |= RaiseInvalidArgument(argumentName);

#include "ErrorStateBuilder.h"

#ifndef BASELIB_ENABLE_INVALID_ARGUMENT_NAME
    #ifdef NDEBUG
        #define BASELIB_ENABLE_INVALID_ARGUMENT_NAME 0
    #else
        #define BASELIB_ENABLE_INVALID_ARGUMENT_NAME 1
    #endif
#endif

BASELIB_CPP_INTERFACE
{
    extern const char* Baselib_StrippedArgumentName;

    // small helper to strong type a pointer
    struct AsPointer
    {
        const void* ptr;
        AsPointer(const void* setPtr)
            : ptr(setPtr)
        {
        }
    };
}


namespace detail
{
    template<typename T>
    static inline ErrorStateBuilder Validate(const T& value, const char* argumentName, Baselib_SourceLocation sourceLocation)
    {
        if (!IsValid(value))
            return ErrorStateBuilder(Baselib_ErrorCode_InvalidArgument, sourceLocation) | WithStaticString(argumentName);
        return ErrorStateBuilder();
    }

    // C++11 compliant way to compare string in compile time
    static inline constexpr bool ConstExprStrEqual(const char* a, const char* b)
    {
        return (*a == *b) && ((*a == '\0') || ConstExprStrEqual(a + 1, b + 1));
    }
}


// Return true if argument is valid
template<typename T>
static inline bool IsValid(const T& value);

static inline bool IsValid(const AsPointer& value)
{
    return value.ptr != nullptr;
}

#if BASELIB_ENABLE_INVALID_ARGUMENT_NAME
    #define Validate(argument) ::detail::Validate((argument), PP_STRINGIZE(argument), BASELIB_SOURCELOCATION)
    #define RaiseInvalidArgument(argument) RaiseError(Baselib_ErrorCode_InvalidArgument) | WithStaticString(PP_STRINGIZE(argument))
#else
    #define Validate(argument) ::detail::Validate((argument), Baselib_StrippedArgumentName, BASELIB_SOURCELOCATION)
    #define RaiseInvalidArgument(argument) RaiseError(Baselib_ErrorCode_InvalidArgument) | WithStaticString(Baselib_StrippedArgumentName)
#endif

// Because C++ order of evaluation is not defined, code executed during construction of ErrorState elements might clear system error value value.
// For example:
// errorState |= RaiseError(...) | WithErrno(errno) | WithSomethingThatCleansErrno()
//
// So we need to ensure that they are captured in separate sequence point, for example:
// const int error = errno;
// errorState |= RaiseError(...) | WithErrno(error) | WithSomethingThatCleansErrno()
//
// This macro verifies that via comparing argument name in compile time.
// Lambda is needed to pass constexpr argument to static_assert.
#define EnsureSystemErrorIsNotPassedDirectly(__type, __value, __name, __prohibitedName) \
    ([](const __type& passThrough) -> __type \
    { \
        static_assert(!::detail::ConstExprStrEqual((__name), __prohibitedName), "cannot pass " __prohibitedName " directly, preserve it in a separate variable first to avoid undefined order of evaluation"); \
        return passThrough; \
    }) (__type ( __value ) )
#define EnsureSystemErrorIsNotPassedDirectlyWithTwoNames(__type, __value, __name, __prohibitedName1, __prohibitedName2) \
    ([](const __type& passThrough) -> __type \
    { \
        static_assert(!::detail::ConstExprStrEqual((__name), __prohibitedName1), "cannot pass " __prohibitedName1 " directly, preserve it in a separate variable first to avoid undefined order of evaluation"); \
        static_assert(!::detail::ConstExprStrEqual((__name), __prohibitedName2), "cannot pass " __prohibitedName2 " directly, preserve it in a separate variable first to avoid undefined order of evaluation"); \
        return passThrough; \
    }) (__type ( __value ) )
