#pragma once

#include <stdio.h>
#include <string.h>
#include <algorithm>

// Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
BASELIB_CPP_INTERFACE
{
    class omemstream
    {
    public:
        omemstream(char* buffer, size_t bufferSize) : m_Buffer(buffer), m_Capacity((int)bufferSize), m_Position(0)
        {
            if (buffer)
                memset(buffer, 0, bufferSize);
        }

        omemstream& operator<<(const char* str)
        {
            formatString("%s", str);
            return *this;
        }

        omemstream& operator<<(int32_t v)
        {
            formatString("%d", v);
            return *this;
        }

        size_t totalBytes() const {return m_Position;}
        bool isValid() const {return m_Position <= m_Capacity;}
        bool formatString(const char* format, ...)
        {
            int maxLen = std::max(m_Capacity - m_Position, 0);

            // if buffer is 0x0 let vsnprintf do a dry run to return number of characters that could've been written
            if (!m_Buffer)
                maxLen = 0;

            va_list args;
            va_start(args, format);
            int result = vsnprintf(m_Buffer + m_Position, maxLen, format, args);
            va_end(args);

            if (result < 0)
                return false;

            int charsGenerated = result;
            m_Position += charsGenerated;

            return isValid();
        }

    private:
        char* m_Buffer;
        int m_Capacity;
        int m_Position;
    };
}
