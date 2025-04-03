#pragma once

class NonCopyable
{
public:
    constexpr NonCopyable() {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};