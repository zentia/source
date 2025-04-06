#pragma once
#include <memory>

#include "module/rhi/rhi_module.h"
#include "module/window/window_module.h"
#include "pipeline/render_pipeline_base.h"
#include "resource/render_resource_base.h"

namespace source_module::render
{
	struct render_init_info
	{
		std::shared_ptr<window::window_module> window_module;
	};
	class render_module
	{
	public:
		void initialize(const render_init_info& init_info);
		void update();
	private:
		void process_swap_data();
		std::shared_ptr<rhi::rhi_module> m_rhi_;
		std::shared_ptr<render_resource_base> m_render_resource_base_;
		std::shared_ptr<render_pipeline_base> m_render_pipeline_base_;
		render_pipeline_type m_render_pipeline_type_ {render_pipeline_type::deferred};
	};
}
