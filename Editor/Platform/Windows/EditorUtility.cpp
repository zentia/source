#include "SourcePrefix.h"
#include "Editor/Platform/Interface/EditorUtility.h"

#include <Windows.h>

#include "Editor/Src/DisplayDialog.h"
#include "WindowsEditorUtility.h"
#include "resource.h"
#include "PlatformDependent/Win/WinUtils.h"

static INT_PTR EditorDisplayDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			
		}
		return TRUE;
	case WM_COMMAND:
		break;
	case WM_DPICHANGED:
		break;
	}

	return FALSE;
}

int DisplayDialogComplexPlatformSpecific(const core::string& title, const core::string& content, const core::string& okButton, const core::string& cancelButton, const core::string& altButton)
{
	HWND mainWindow = GetMainEditorWindow();
	int res = DialogBoxW(winutils::GetInstanceHandle(), MAKEINTRESOURCEW(altButton.empty() ? IDD_DIALOGTWO : IDD_DIALOGTHREE), mainWindow, (DLGPROC)EditorDisplayDialogProc);
	return 0;
}