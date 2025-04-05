#include "render_module.h"

#include "global_rendering.h"
#include "Modules/asset/asset_module.h"
#include "modules/config/config_module.h"
#include "Modules/rhi/vulkan/vulkan_rhi.h"
#include "resource/render_resource.h"
#include "runtime/application/application.h"
#include "swap_context/render_swap_context.h"

namespace source_module::render
{
	void render_module::initialize(const render_init_info& init_info)
	{
		const std::shared_ptr<source_runtime::config_module> config_module = source_runtime::application::instance()->m_config_module;
		assert(config_module);

		// render context initialize
		rhi::rhi_init_info rhi_init_info;
		rhi_init_info.window_module = init_info.window_module;

		m_rhi_ = std::make_shared<rhi::vulkan_rhi>();
		m_rhi_->initialize(rhi_init_info);

		// global rendering resource
		const std::string& global_rendering_res_url = config_module->get_global_rendering_res_url();
		const source_runtime::global_rendering_res global_rendering_res = source_runtime::application::instance()->m_asset_module->load_asset<source_runtime::global_rendering_res>(global_rendering_res_url);

		// upload ibl, color grading textures
		scene_resource_desc scene_resource_desc;
		scene_resource_desc.m_ibl_resource_desc.m_sky_box_irradiance_map = global_rendering_res.m_sky_box_irradiance_map;
		scene_resource_desc.m_ibl_resource_desc.m_sky_box_specular_map = global_rendering_res.m_sky_box_specular_map;
		scene_resource_desc.m_ibl_resource_desc.m_brdf_map = global_rendering_res.m_brdf_map;
		scene_resource_desc.m_color_grading_resource_desc.m_color_grading_map = global_rendering_res.m_color_grading_map;

		m_render_resource_base_ = std::make_shared<render_resource>();

	}

	void render_module::update()
	{
		process_swap_data();
	}

	void render_module::process_swap_data()
	{
		if (m_render_pipeline_type_ == render_pipeline_type::deferred)
		{
			m_render_pipeline_base_->deferred_render(m_rhi_, m_render_resource_base_);
		}
		else if (m_render_pipeline_type_ == render_pipeline_type::forward)
		{
			m_render_pipeline_base_->forward_render(m_rhi_, m_render_resource_base_);
		}
		
	}

}
