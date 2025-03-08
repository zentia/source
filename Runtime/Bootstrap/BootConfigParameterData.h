#pragma once

#include "BootConfigData.h"

namespace BootConfig
{
	class LinkedParameter
	{
	public:
		LinkedParameter(const char* name) : m_Name(name)
		{
			
		}

		LinkedParameter* nextParameter;
		const char* m_Name;
	};

	template<typename T>
	class ParameterData : public LinkedParameter
	{
	public:
		ParameterData(const Data& data, const char* name, T defaultValue)
			: LinkedParameter(name)
		{
			
		}
	};
}
