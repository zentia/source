#pragma once
#include "canvas/canvas.h"
#include "module/render/render_module.h"
#include "module/rhi/rhi_module.h"
#include "module/window/window_module.h"

namespace source_runtime::ui
{
	struct window_ui_init_info
	{
		std::shared_ptr<source_module::window::window_module> window_module;
		std::shared_ptr<source_module::render::render_module> render_module;
	};

	class ui_module
	{
	public:
		virtual void initialize(window_ui_init_info init_info) = 0;
		virtual void update();
	private:
		canvas* m_canvas_{ nullptr };
		std::shared_ptr<source_module::rhi::rhi_module> m_rhi_;
	};
}
