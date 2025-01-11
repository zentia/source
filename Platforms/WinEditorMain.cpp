#include "OpenSourcePrefix.h"

#include <iostream>
#include <Windows.h>

int32_t LanunchWindowsStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int32_t nCmdShow, const TCHAR* CmdLine)
{
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    int32_t result = LanunchWindowsStartup(hInst, hInstPrev, cmdline, cmdshow, nullptr);
    return 0;
}