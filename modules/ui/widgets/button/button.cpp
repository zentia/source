#include "button.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace source_runtime::ui
{
	button::button(const std::string&& label, const aiVector2D&& size, const bool disabled) : m_label(label), m_size(size), m_disable(disabled)
	{
		
	}

	void button::draw_impl()
	{
		const auto& style = ImGui::GetStyle();

		auto normal_color = style.Colors[ImGuiCol_Button];

		if (ImGui::ButtonEx(m_label.c_str(), ImVec2(m_size.x, m_size.y), m_disable ? ImGuiItemFlags_Disabled : 0))
		{
			m_on_click();
		}
	}

}
