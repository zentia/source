#pragma once
#include "render_pipeline_base.h"

namespace source_module::render
{
	class render_pipeline : public render_pipeline_base
	{
	public:
		void deferred_render(std::shared_ptr<rhi::rhi_module> rhi_module, std::shared_ptr<render_resource_base> render_resource_base) final;
	};
}
