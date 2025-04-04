#pragma once
#include "modules/ui/panel/panel_window.h"

namespace source_editor::hub
{
	class hub_panel : public source_runtime::ui::panel_window
	{
	public:
		hub_panel();
		void draw() override;
	};
}
