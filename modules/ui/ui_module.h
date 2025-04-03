#pragma once
#include "canvas/canvas.h"
#include "Modules/render/render_module.h"
#include "Modules/rhi/rhi_module.h"
#include "Modules/window/window_module.h"

namespace source_runtime::ui
{
	struct window_ui_init_info
	{
		std::shared_ptr<window_module> window_module;
		std::shared_ptr<source_module::render::render_module> render_module;
	};

	class ui_module
	{
	public:
		virtual void initialize(window_ui_init_info init_info) = 0;
		virtual void render();
	private:
		canvas* m_canvas_{ nullptr };
		std::shared_ptr<source_module::rhi::rhi> m_rhi_;
	};
}
