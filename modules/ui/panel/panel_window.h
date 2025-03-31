#pragma once
#include "panel_transformable.h"

namespace source_runtime::ui
{
	class panel_window : public panel_transformable
	{
	public:
		panel_window();
		[[nodiscard]]bool is_opened() const
		{
			return m_opened_;
		}
	private:
		bool m_opened_{ false };
	};
}
