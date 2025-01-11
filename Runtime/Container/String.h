#pragma once

namespace OpenSource
{
    class String
    {
    public:
        String() = default;
        String(String&&) = default;
        String(const String&) = default;
        String& operator=(String&&) = default;
        String& operator=(const String&) = default;
    };
}
