#include "panel_window.h"

namespace source_runtime::ui
{
	panel_window::panel_window(const std::string&& name, const bool opened) : m_name(name), m_opened_(opened)
	{
		
	}

	void panel_window::open()
	{
		if (!m_opened_)
		{
			m_opened_ = true;
		}
	}

}
