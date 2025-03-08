#pragma once

#include "Include/C/Baselib_Memory.h"
#include "Baselib_Utilities.h"
#include "Include/External/utfcpp/source/utf8/unchecked.h"

namespace detail
{
    // Default memory allocation methods
    struct default_allocator_impl
    {
        static void* allocate(size_t size)
        {
            return ::Baselib_Memory_Allocate(size);
        }

        static void free(void* ptr)
        {
            ::Baselib_Memory_Free(ptr);
        }
    };

    template<class T, size_t CAPACITY, class allocator_impl = default_allocator_impl>
    class string_container
    {
    public:
        using value_type = T;
        using pointer = value_type *;

        explicit string_container() : m_Buffer(m_InternalBuffer), m_Capacity(CAPACITY), m_Size(0)
        {
            BaselibAssert(m_Capacity > 0, "Capacity must not be 0");
        }

        ~string_container()
        {
            destroy_buffer();
        }

        void push_back(const value_type& data)
        {
            if (m_Size < (m_Capacity - 1)) // -1 because we need to fit a 0-term character
            {
                m_Buffer[m_Size++] = data;
            }
            else
            {
                // buffer is full, allocate memory for a new buffer twice the capacity
                size_t newCapacity = m_Capacity * 2;
                pointer newBuffer = (pointer)allocator_impl::allocate(sizeof(value_type) * newCapacity);
                BaselibAssert(newBuffer != nullptr, "Memory allocation failed");

                std::memcpy(newBuffer, m_Buffer, m_Size * sizeof(value_type));
                destroy_buffer();

                m_Buffer = newBuffer;
                m_Capacity = newCapacity;

                push_back(data);
            }
        }

        pointer c_str()
        {
            m_Buffer[m_Size] = '\0';
            return m_Buffer;
        }

        size_t capacity() const
        {
            return m_Capacity;
        }

        size_t size() const
        {
            return m_Size;
        }

    private:
        void destroy_buffer()
        {
            // free memory only if it was allocated
            if (m_Buffer != m_InternalBuffer)
            {
                allocator_impl::free(m_Buffer);
                m_Buffer = nullptr;
            }
        }

        value_type m_InternalBuffer[CAPACITY];
        pointer m_Buffer;
        size_t m_Capacity;
        size_t m_Size;
    };

    typedef string_container<char, 128>             string_container_128byte_utf8;
    typedef string_container<baselib_char16_t, 128> string_container_128byte_utf16;

    template<typename char16_type, typename T>
    static inline const char* StrUtf16ToUtf8(const char16_type* utf16String, size_t length, T& outUtf8)
    {
        utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(outUtf8));
        return outUtf8.c_str();
    }

    template<typename char16_type, typename T>
    static inline const char* StrUtf16ToUtf8(const char16_type* utf16String, T& outUtf8)
    {
        size_t length = StrLenUtf16(utf16String);
        return StrUtf16ToUtf8(utf16String, length, outUtf8);
    }

    template<typename T>
    static inline const baselib_char16_t* StrUtf8ToUtf16(const char* utf8String, size_t length, T& outUtf16)
    {
        if (utf8::is_valid(utf8String, utf8String + length))
        {
            utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(outUtf16));
        }

        return static_cast<const baselib_char16_t*>(outUtf16.c_str());
    }

    template<typename T>
    static inline const baselib_char16_t* StrUtf8ToUtf16(const char* utf8String, T& outUtf16)
    {
        return StrUtf8ToUtf16(utf8String, strlen(utf8String), outUtf16);
    }
}
