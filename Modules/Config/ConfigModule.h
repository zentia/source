#pragma once

#include <filesystem>
#include <rfl.hpp>

namespace source_runtime
{
	class config
	{
	/*public:
		config() = default;
		config(config&& c) = default;
		config(config& c) = default;
		~config() = default;
		config& operator=(const config& c) = default;
		config& operator=(config&& c) = default;*/
	public:
		std::filesystem::path get_root_folder()
		{
			return m_project_folder.value() / m_root_folder;
		}
		std::string m_asset_folder;
		std::string m_default_world_url;
		std::string m_editor_font_path;
		std::filesystem::path m_root_folder;
		rfl::Skip<std::filesystem::path> m_project_folder;
		std::string m_global_render_res_url;
		std::string m_global_particle_res_url;
	};
	class config_module
	{
	public:
		void initialize(const std::filesystem::path& config_file_path);
		std::filesystem::path get_root_folder();
		std::string& get_global_rendering_res_url();
		std::string& get_global_particle_res_url();
	private:
		config m_config_;
	};
}
