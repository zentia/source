#include "asset_module.h"

#include <filesystem>

#include "Editor/base/Application/application.h"

namespace source_runtime
{
	std::filesystem::path asset_module::get_full_path(const std::string& relative_path)
	{
		return absolute(application::instance()->m_config_module->get_root_folder() / relative_path);
	}

}
