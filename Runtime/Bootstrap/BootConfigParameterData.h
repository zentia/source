#pragma once

#include "BootConfigData.h"

namespace BootConfig
{
	template<typename T>
	class ParameterParser
	{
	public:
		T Parse(const char* value, T defaultValue) const;
	};

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
			, m_Data(data)
			, m_DefaultValue(defaultValue)
		{
			
		}

		T operator[](int index) const
		{
			if (!m_Data.HasKey(m_Name))
				return m_DefaultValue;
			return m_Parser.Parse(m_Data.GetValue(m_Name, index), m_DefaultValue);
		}

		operator T() const
		{
			return (*this)[0];
		}
	private:
		const Data& m_Data;
		const T m_DefaultValue;
		const ParameterParser<T> m_Parser;
	};
}
