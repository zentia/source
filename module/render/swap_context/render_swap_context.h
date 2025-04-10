#pragma once
#include <optional>

#include "module/render/global_rendering.h"

namespace source_module::render
{
	struct scene_ibl_resource_desc
	{
		source_runtime::sky_box_irradiance_map m_sky_box_irradiance_map;
		source_runtime::sky_box_specular_map m_sky_box_specular_map;
		std::string m_brdf_map;
	};
	struct scene_color_grading_resource_desc
	{
		std::string m_color_grading_map;
	};
	struct scene_resource_desc
	{
		scene_ibl_resource_desc m_ibl_resource_desc;
		scene_color_grading_resource_desc m_color_grading_resource_desc;
	};
	struct camera_swap_data
	{
		std::optional<float> m_fov_x;
		//std::optional<render>
	};
	struct render_swap_data
	{
		
	};

	class render_swap_context
	{
	public:
		
	};
}
