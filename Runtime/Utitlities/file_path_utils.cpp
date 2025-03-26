#include "file_path_utils.h"

namespace source_runtime
{
	std::string get_directory(const std::string& path)
	{
		int index = path.find_last_of("\\");
		return path.substr(index);
	}

}
