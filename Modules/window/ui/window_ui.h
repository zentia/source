#pragma once
#include "Modules/render/render_module.h"
#include "Modules/window/window_module.h"

namespace source_runtime
{
	struct window_ui_init_info
	{
		std::shared_ptr<window_module> window_module;
		std::shared_ptr<render_module> render_module;
	};

	class window_ui
	{
	public:
		virtual void initialize(window_ui_init_info init_info) = 0;
		virtual void pre_render() = 0;
	};
}
