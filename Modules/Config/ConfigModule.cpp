#include "ConfigModule.h"

#include <filesystem>
#include <fstream>
//#include <rfl/json.hpp>
//#include <rfl.hpp>

namespace source_runtime
{
	void config_module::initialize(const std::filesystem::path& config_file_path)
	{
		const std::ifstream config_file(config_file_path);
		/*std::stringstream str_stream;
		str_stream << config_file.rdbuf();*/
		//m_config_ = rfl::json::read<config>(str_stream).value();
	}

}
