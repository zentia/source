#include "SourcePrefix.h"

#include "WindowsPlatformMisc.h"

#include "Editor/Platform/Windows/resource.h"
#include "Source/Baselib_Config_Common_WinApiBased.inl.h"


class WindowsDialog
{
public:
	WindowsDialog(const core::string& text, const core::string& caption)
		: m_Text(text)
		, m_Caption(caption)
		, m_CancelButtonEnabled(false)
		, m_Result(AppReturnType::Ok)
		, m_DialogHwnd(nullptr)
	{

	}
	static AppReturnType::Type Show(HWND parentWindowHandle, AppMsgType::Type msgType, const core::string& text, const core::string& caption)
	{
		WindowsDialog instance(text, caption);
		switch (msgType)
		{
		case AppMsgType::Ok:
		{
			instance.m_CancelButtonEnabled = false;
			instance.m_Result = AppReturnType::Ok;
			return instance.Show(parentWindowHandle);
		}
		}
	}
private:
	AppReturnType::Type Show(HWND parentWindowHandle)
	{
		m_DialogHwnd = CreateDialogParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_DIALOGONE), parentWindowHandle, [](HWND handleWnd, uint32_t message, WPARAM wparam, LPARAM lparam)
			{
				WindowsDialog* instance;
				if (message == WM_INITDIALOG)
				{
					instance = (WindowsDialog*)lparam;
				}
				else
				{
					instance = (WindowsDialog*)GetWindowLongPtrW(handleWnd, DWLP_USER);
				}
				return instance->MessageBoxDlgProc(handleWnd, message, wparam, lparam);
			}, (LPARAM)this);
	}

	intptr_t MessageBoxDlgProc(HWND handleWnd, uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
			return OnInitDialog();
		case WM_DESTROY:
			return true;
		}
	}

	bool OnInitDialog()
	{
		return true;
	}
	core::string m_Text;
	core::string m_Caption;
	bool m_CancelButtonEnabled;
	AppReturnType::Type m_Result;
	HWND m_DialogHwnd;
};

AppReturnType::Type WindowsPlatformMisc::MessageBoxExt(AppMsgType::Type msgType, const wchar_t* text, const wchar_t* caption)
{

}
