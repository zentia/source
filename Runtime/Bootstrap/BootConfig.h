#include "BootConfigParameterData.h"

namespace BootConfig
{
	static const char kFilename[] = "boot.config";

	const Data& GetGlobalConfig();

	template<typename T>
	class Parameter : public ParameterData<T>
	{
	public:
		Parameter(const char* name, T defaultValue):ParameterData<T>(GetGlobalConfig(), name, defaultValue){}
	};
}
