#pragma once
#include <memory>

#include "render_resource_base.h"
#include "Modules/rhi/rhi_module.h"

namespace source_module::render
{
	class render_resource : public render_resource_base
	{
	public:
		void upload_global_render_resource(std::shared_ptr<rhi::rhi> rhi, source_runtime::scene_resource_desc scene_resource_desc) override;
	private:
	};
}
