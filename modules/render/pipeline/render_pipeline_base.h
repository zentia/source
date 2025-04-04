#pragma once
#include <memory>

#include "modules/render/resource/render_resource_base.h"
#include "modules/rhi/rhi_module.h"

namespace source_module::render
{
	class render_pipeline_base
	{
	public:
		virtual void forward_render(std::shared_ptr<rhi::rhi_module> rhi_module, std::shared_ptr<render_resource_base> render_resource_base);
		virtual void deferred_render(std::shared_ptr<rhi::rhi_module> rhi_module, std::shared_ptr<render_resource_base> render_resource_base);
	};
}
