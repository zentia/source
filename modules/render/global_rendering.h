#pragma once
#include <string>

#include "Modules/camera/camera_config.h"
#include "Runtime/math/color.h"
#include "Runtime/math/vector3.h"

namespace source_runtime
{
	// 辐照度
	class sky_box_irradiance_map
	{
	public:
		std::string m_negative_x_map;
		std::string m_positive_x_map;
		std::string m_negative_y_map;
		std::string m_positive_y_map;
		std::string m_negative_z_map;
		std::string m_positive_z_map;
	};

	class sky_box_specular_map
	{
	public:
		std::string m_negative_x_map;
		std::string m_positive_x_map;
		std::string m_negative_y_map;
		std::string m_positive_y_map;
		std::string m_negative_z_map;
		std::string m_positive_z_map;
	};

	class directional_light
	{
	public:
		math::vector3 m_direction;
		math::color m_color;
	};

	class global_rendering_res
	{
	public:
		bool m_enable_fxaa;
		sky_box_irradiance_map m_sky_box_irradiance_map;
		sky_box_specular_map m_sky_box_specular_map;
		std::string m_brdf_map;
		std::string m_color_grading_map;

		math::color m_sky_color;
		math::color m_ambient_light;
		camera_config m_camera_config;
		directional_light m_directional_light;
	};
}
