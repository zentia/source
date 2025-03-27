#pragma once
#include "global_rendering.h"

namespace source_runtime
{
	struct level_ibl_resource_desc
	{
		sky_box_irradiance_map m_sky_box_irradiance_map;
		sky_box_specular_map m_sky_box_specular_map;
		std::string m_brdf_map;
	};
	struct level_color_grading_resource_desc
	{
		std::string m_color_grading_map;
	};
	struct level_resource_desc
	{
		level_ibl_resource_desc m_level_ibl_resource_desc;
		level_color_grading_resource_desc m_color_grading_resource_desc;
	};
	struct camera_swap_data
	{
		std::optional<float> m_fov_x;
		//std::optional<render>
	};
	struct render_swap_data
	{
		
	};
}
