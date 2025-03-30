#include "window_module.h"

namespace source_runtime
{
	window_module::~window_module()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void window_module::Initialize(WindowCreateInfo createInfo)
	{
		if (!glfwInit())
		{
			FatalErrorMsg("failed to initialize GLFW");
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title, nullptr, nullptr);
		if (!m_Window)
		{
			FatalErrorMsg("failed to create window");
			glfwTerminate();
			return;
		}

		// Setup input callbacks
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetKeyCallback(m_Window, key_callback);
	}

	void window_module::poll_events() const
	{
		glfwPollEvents();
	}

	bool window_module::should_close() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void window_module::set_should_close(const bool value) const
	{
		glfwSetWindowShouldClose(m_Window, value);
	}

	void window_module::set_title(const char* title) const
	{
		glfwSetWindowTitle(m_Window, title);
	}

	GLFWwindow* window_module::get_window() const
	{
		return m_Window;
	}

	std::array<int, 2> window_module::get_window_size() const
	{
		return std::array<int, 2>({m_width_, m_height_});
	}

}
