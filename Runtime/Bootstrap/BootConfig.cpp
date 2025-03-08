#include "SourcePrefix.h"

#include "BootConfig.h"
#include "BootConfigData.h"
#include <stdarg.h>
#if _MSC_VER
#include <malloc.h>
#endif

namespace BootConfig
{
	namespace Internal
	{
		BootConfig::Data s_Data;

		BootConfig::Data& GetGlobalConfig()
		{
			return s_Data;
		}
	}
	const Data& GetGlobalConfig()
	{
		return BootConfig::Internal::GetGlobalConfig();
	}

}
