#include "asset_module.h"

#include <filesystem>

namespace source_module::asset
{
	asset_module::asset_module(std::filesystem::path&& root_folder) 
	{
		m_root_folder_ = std::move(root_folder);
	}

	std::filesystem::path asset_module::get_full_path(const std::string& relative_path) const
	{
		return absolute(m_root_folder_ / relative_path);
	}

}
