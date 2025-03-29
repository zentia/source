#include "editor_ui.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Editor/base/Application/application.h"

namespace source_editor
{
	inline void window_content_scale_callback(GLFWwindow* window, float x_scale, float y_scale)
	{
		
	}
	void editor_ui::initialize(source_runtime::window_ui_init_info init_info)
	{
		std::shared_ptr<source_runtime::config_module> config_module = get_application().m_config_module;
		assert(config_module);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		float x_scale, y_scale;
		glfwGetWindowContentScale(init_info.window_module->get_window(), &x_scale, &y_scale);
		float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		//io.Fonts->AddFontFromFileTTF();
	}

	void editor_ui::pre_render()
	{
		show_editor_ui();
	}

	void editor_ui::show_editor_ui()
	{
		show_editor_menu(&m_editor_menu_window_open_);
	}

	void editor_ui::show_editor_menu(bool* open)
	{
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	}

}
