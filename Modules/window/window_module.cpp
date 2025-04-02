#include "window_module.h"

//#include "spdlog/spdlog.h"

namespace source_runtime
{
	window_module::window_module(/*luisa::compute::Device& device, luisa::compute::Stream& stream*/) /*: ImGuiWindow(device, stream, "source")*/
	{
		
	}

	window_module::~window_module()
	{
		glfwDestroyWindow(m_window_);
		glfwTerminate();
	}

	void window_module::Initialize(WindowCreateInfo createInfo)
	{
		if (!glfwInit())
		{
			//SPDLOG_ERROR("failed to initialize GLFW");
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_window_ = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title, nullptr, nullptr);
		if (!m_window_)
		{
			//SPDLOG_ERROR("failed to create window");
			glfwTerminate();
			return;
		}

		// Setup input callbacks
		glfwSetWindowUserPointer(m_window_, this);
		glfwSetKeyCallback(m_window_, key_callback);
	}

	bool window_module::should_close() const
	{
		return glfwWindowShouldClose(m_window_);
	}

	void window_module::set_should_close(const bool value) const
	{
		glfwSetWindowShouldClose(m_window_, value);
	}

	void window_module::set_title(const char* title) const
	{
		glfwSetWindowTitle(m_window_, title);
	}

	GLFWwindow* window_module::get_window() const
	{
		return m_window_;
	}

	glm::vec2 window_module::get_window_size() const
	{
		return glm::vec2({m_width_, m_height_});
	}

}
