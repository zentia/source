#pragma once

#include <filesystem>

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
		std::string m_asset_folder;
		std::string m_default_world_url;
		std::string m_editor_font_path;
	};
	class config_module
	{
	public:
		void initialize(const std::filesystem::path& config_file_path);
	private:
		config m_config_;
	};
}
