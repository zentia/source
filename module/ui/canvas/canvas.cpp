#include "canvas.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_dx12.h>

#include "imgui_impl_glfw.h"
#include "runtime/application/application.h"

namespace source_runtime::ui
{
	void canvas::draw()
	{
		if (!m_panels_.empty())
		{
			if (application::instance()->m_rhi_module->get_interface_type() == source_module::rhi::rhi_interface_type::vulkan)
			{
				ImGui_ImplVulkan_NewFrame();
			}
			else
			{
				ImGui_ImplDX12_NewFrame();
			}
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			if (m_is_dock_space_)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

				ImGui::Begin("##dock_space", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);// | ImGuiWindowFlags_NoDocking);
				const ImGuiID dock_space_id = ImGui::GetID("dock_space_id");
				ImGui::DockSpace(dock_space_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
				ImGui::SetWindowPos({ 0.f, 0.f });
				const ImVec2 display_size = ImGui::GetIO().DisplaySize;
				ImGui::SetWindowSize(display_size);
				ImGui::End();

				ImGui::PopStyleVar(1);
			}

			for (auto& p : m_panels_)
			{
				p.get().draw();
			}

			ImGui::Render();
		}
	}

}
