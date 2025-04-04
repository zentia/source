#pragma once
#include <memory>

#include "render_resource_base.h"
#include "Modules/rhi/rhi_module.h"

namespace source_module::render
{
	struct ibl_resource
	{
		rhi::rhi_image* brdf_lut_texture_image;
		rhi::rhi_image_view* brdf_lut_texture_image_view;
		rhi::rhi_sampler* brdf_lut_texture_sampler;
	};
	struct global_render_resource
	{
		
	};
	class render_resource : public render_resource_base
	{
	public:
		~render_resource() override = default;
		void upload_global_render_resource(std::shared_ptr<rhi::rhi_module> rhi, scene_resource_desc scene_resource_desc) override;
	private:
	};
}
