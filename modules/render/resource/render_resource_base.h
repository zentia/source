#pragma once
#include "Modules/render/render_swap_context.h"
#include "Modules/rhi/rhi_module.h"

namespace source_module::render
{
	class render_resource_base
	{
	public:
		virtual void upload_global_render_resource(std::shared_ptr<rhi::rhi> rhi, source_runtime::scene_resource_desc scene_resource_desc) = 0;
	};
}
