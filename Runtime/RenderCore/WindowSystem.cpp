#include "WindowSystem.h"

namespace Source
{
	WindowSystem::~WindowSystem()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Source::WindowSystem::Initialize(WindowCreateInfo createInfo)
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

	bool Source::WindowSystem::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	GLFWwindow* WindowSystem::GetWindow() const
	{
		return m_Window;
	}

	std::array<int, 2> WindowSystem::GetWindowSize() const
	{
		return std::array<int, 2>({m_width_, m_height_});
	}

}
