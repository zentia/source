#pragma once
#include "Modules/window/ui/window_ui.h"

namespace source_editor
{
	class editor_ui : public source_runtime::window_ui
	{
	public:
		void initialize(source_runtime::window_ui_init_info init_info) override;
		void pre_render() override;
	private:
		void show_editor_ui();
		void show_editor_menu(bool* open);
		bool m_editor_menu_window_open_ = true;
	};

}
