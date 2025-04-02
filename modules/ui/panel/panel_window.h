#pragma once

#include "panel_transformable.h"
#include <string>

namespace source_runtime::ui
{
	class panel_window : public panel_transformable
	{
	public:
		panel_window(const std::string&& name = "", bool opened = true);
		[[nodiscard]]bool is_opened() const
		{
			return m_opened_;
		}
		void open();
		std::string m_name;
	private:
		bool m_opened_{ false };
	};
}
