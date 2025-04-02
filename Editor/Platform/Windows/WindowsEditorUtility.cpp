#include <Windows.h>

static HWND gMainWindow = nullptr;
HWND GetMainEditorWindow()
{
	return gMainWindow;
}