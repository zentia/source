#pragma once

class ContainerWindow
{
public:
    void Init();
};

#if PLATFORM_WIN

constexpr const wchar_t* kContainerWindowsClassName = L"OpenSourceContainerWndClass";
#endif