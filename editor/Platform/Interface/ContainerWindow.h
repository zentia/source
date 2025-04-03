#pragma once

class ContainerWindow
{
public:
    enum ShowMode
    {
        kShowNormalWindow = 0,
        kShowPopupMenu = 1,
        kShowUtility = 2,
        kShowNoShadow = 3,
        kShowMainWindow = 4,
	    kShowAuxWindow = 5,
        kShowTooltip = 6,
        kShowModalUtility = 7,
    };
    void Init(int showMode);

private:
    ShowMode m_ShowMode;
};

#if PLATFORM_WIN

constexpr const wchar_t* kContainerWindowsClassName = L"OpenSourceContainerWndClass";
#endif