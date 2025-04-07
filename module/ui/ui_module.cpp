#include "ui_module.h"

#include <imgui.h>

#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"
#include "module/rhi/d3d12/d3d12_rhi.h"
#include "module/rhi/vulkan/vulkan_rhi.h"
#include "runtime/application/application.h"

namespace source_runtime::ui
{
	void ui_module::initialize(window_ui_init_info init_info)
	{
		m_is_vulkan_ = m_rhi_->get_interface_type() == source_module::rhi::rhi_interface_type::vulkan;
	}

	void ui_module::update()
	{
		if (m_canvas_)
		{
			if (m_is_vulkan_)
			{
				ImGui_ImplVulkan_NewFrame();
			}
			else
			{
				ImGui_ImplDX12_NewFrame();
			}
			m_canvas_->draw();
			if (m_is_vulkan_)
			{
				ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<source_module::rhi::vulkan_rhi>(m_rhi_)->m_vk_command_buffer);
			}
			else
			{
				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<source_module::rhi::d3d12_rhi>(m_rhi_)->m_graphics_command_list);
			}
		}
	}

}
