#pragma once
#include "modules/ui/ui_module.h"

namespace source_editor
{
	class editor_ui : public source_runtime::ui::ui_module
	{
	public:
		void initialize(source_runtime::ui::window_ui_init_info init_info) override;
		void update() override;
	private:
		void show_editor_ui();
		void show_editor_menu(bool* open);
		void set_ui_color_style();
		bool m_editor_menu_window_open_ = true;
	};

}
