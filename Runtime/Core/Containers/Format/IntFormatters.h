#pragma once

namespace core
{
	void FormatValueTo(core::string& outputString, const signed char& value, core::string_ref formatString = core::string_ref());
	void FormatValueTo(core::string& outputString, const unsigned char& value, core::string_ref formatString = core::string_ref());
}
