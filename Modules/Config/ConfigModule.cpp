#include "ConfigModule.h"

#include <filesystem>
#include <fstream>
#include <rfl/json.hpp>
#include <rfl.hpp>

namespace source_runtime
{
	void config_module::initialize(const std::filesystem::path& config_file_path)
	{
		if (exists(config_file_path))
		{
			const std::ifstream config_file(config_file_path);
			std::stringstream str_stream;
			str_stream << config_file.rdbuf();
			m_config_ = rfl::json::read<config>(str_stream).value();
		}
		m_config_.m_project_folder = config_file_path.parent_path();
	}

	std::filesystem::path config_module::get_root_folder()
	{
		return m_config_.get_root_folder();
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
