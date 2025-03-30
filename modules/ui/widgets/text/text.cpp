#include "text.h"

#include <imgui.h>

namespace source_runtime::ui
{
	text::text(const std::string&& content) : m_content(content)
	{
		
	}

	void text::draw_impl()
	{
		ImGui::Text(m_content.c_str());
	}

}
