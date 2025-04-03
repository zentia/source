#pragma once
#include "Modules/camera/camera_config.h"
#include "Runtime/Math/color.h"
#include "Runtime/Math/vector3.h"

namespace source_runtime
{
	// ·øÕÕ¶È
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
		vector3 m_direction;
		color m_color;
	};

	class global_rendering_res
	{
	public:
		bool m_enable_fxaa;
		sky_box_irradiance_map m_sky_box_irradiance_map;
		sky_box_specular_map m_sky_box_specular_map;
		std::string m_brdf_map;
		std::string m_color_grading_map;

		color m_sky_color;
		color m_ambient_light;
		camera_config m_camera_config;
		directional_light m_directional_light;
	};
}
