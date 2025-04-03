#pragma once
#include <memory>

#include "Modules/rhi/rhi_module.h"
#include "Modules/window/window_module.h"
#include "pipeline/render_pipeline_base.h"
#include "resource/render_resource_base.h"

namespace source_module::render
{
	struct render_init_info
	{
		std::shared_ptr<source_runtime::window_module> window_module;
	};
	class render_module
	{
	public:
		void initialize(const render_init_info& init_info);
		void tick(float delta_time);
	private:
		void process_swap_data();
		std::shared_ptr<rhi::rhi> m_rhi_;
		std::shared_ptr<render_resource_base> m_render_resource_base_;
		std::shared_ptr<render_pipeline_base> m_render_pipeline_base_;
	};
}
