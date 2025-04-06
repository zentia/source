#pragma once
#include "module/render/swap_context/render_swap_context.h"
#include "module/rhi/rhi_module.h"

namespace source_module::render
{
	class render_resource_base
	{
	public:
		virtual ~render_resource_base() {}
		virtual void upload_global_render_resource(std::shared_ptr<rhi::rhi_module> rhi, scene_resource_desc scene_resource_desc) = 0;
	};
}
