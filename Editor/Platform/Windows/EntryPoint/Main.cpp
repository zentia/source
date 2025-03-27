#include <filesystem>

#include "WindowsIncludes.h"
#include "Editor/Platform/Windows/WinEditorMain.h"

#include <string>
#include <string_view>

extern "C" IMAGE_DOS_HEADER __ImageBase;

std::wstring GetExecutablePath()
{
    std::wstring exePath;
    DWORD exePathLength = 0;
    do
    {
        exePath.resize(exePath.length() + MAX_PATH);
        exePathLength = GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), &exePath[0], static_cast<DWORD>(exePath.size()));
    }
    while (exePathLength >= exePath.length());

    exePath.resize(exePathLength);
    return exePath;
}

void DisplayFailedToLoadSourceError(LPWSTR cmdLine)
{
	// MessageBoxW(nullptr, )
}

int main(int argc, char** argv)
{

    auto sourceDll = LoadLibraryEx("Source.dll", nullptr, 0);
    if (sourceDll == nullptr)
    {
        auto error = GetLastError();
        return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, error);
    }

    auto sourceMain = reinterpret_cast<decltype(SourceMain)*>(GetProcAddress(sourceDll, "SourceMain"));
    if (sourceMain == nullptr)
    {
        auto error = GetLastError();
        return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, error);
    }
    return sourceMain(argc, argv);
}