#include "ui_module.h"

#include <imgui.h>

#include "imgui_impl_vulkan.h"
#include "Modules/rhi/vulkan/vulkan_rhi.h"

namespace source_runtime::ui
{
	void ui_module::render()
	{
		if (m_canvas_)
		{
			m_canvas_->draw();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<source_module::rhi::vulkan_rhi>(m_rhi_)->m_vk_command_buffer);
		}
	}

}
