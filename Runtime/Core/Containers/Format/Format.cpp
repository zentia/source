#include "SourcePrefix.h"
#include "Format.h"

namespace core
{
	namespace detail
	{
		void FormatToInternal(string& output, core::string_ref formatStr, const FormatArgs& args)
		{
			
		}

		core::string FormatInternal(string_ref formatStr, const FormatArgs& args)
		{
			string output(kMemTempAlloc);
			FormatToInternal(output, formatStr, args);
			return output;
		}

	}
}
