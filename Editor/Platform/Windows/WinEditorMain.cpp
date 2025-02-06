#include "SourcePrefix.h"
#include "WinEditorMain.h"

#include "Editor/Src/Application/Application.h"

#include <iostream>
#include <Windows.h>

int32_t LanunchWindowsStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int32_t nCmdShow, const TCHAR* CmdLine)
{
    return 0;
}

extern "C" SOURCE_API int SourceMain()
{
    new Application();
    GetApplication().InitializeProject();
    return 0;
}