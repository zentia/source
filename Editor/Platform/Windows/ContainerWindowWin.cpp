#include "SourcePrefix.h"
#include "Editor/Platform/Interface/ContainerWindow.h"
#include <Windows.h>
#include <windowsx.h>

void ContainerWindow::Init(int showMode)
{

    m_ShowMode = static_cast<ShowMode>(showMode);
    DWORD windowStyle = 0;
    DWORD extendedStyle = 0;
    LPCWSTR windowClass = kContainerWindowsClassName;
}