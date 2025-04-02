#include "hub_panel.h"

#include <imgui.h>

namespace source_editor::hub
{
	hub_panel::hub_panel() : panel_window("hub")
	{
		
	}

	void hub_panel::draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 50.0f,50.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

		panel_window::draw();

		ImGui::PopStyleVar(2);
	}

}
