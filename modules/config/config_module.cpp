#include "config_module.h"

#include <filesystem>
#include <fstream>
#include <rfl/json.hpp>
#include "Modules/asset/asset_module.h"

namespace source_runtime
{
	void config_module::initialize(const std::filesystem::path& config_file_path)
	{
		if (exists(config_file_path))
		{
			m_config_ = source_module::asset::asset_module::load_config<config>(config_file_path.generic_string());
		}
		m_config_.m_project_folder = config_file_path.parent_path();
	}

	std::filesystem::path config_module::get_root_folder()
	{
		return m_config_.get_root_folder();
	}

	std::filesystem::path config_module::get_editor_font_path() const
	{
		return m_config_.m_root_folder / m_config_.m_editor_font_path;
	}

	std::string& config_module::get_global_rendering_res_url()
	{
		return m_config_.m_global_render_res_url;
	}

	std::string& config_module::get_global_particle_res_url()
	{
		return m_config_.m_global_particle_res_url;
	}
}
