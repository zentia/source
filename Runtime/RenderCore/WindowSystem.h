#pragma once

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

namespace Source
{
	struct WindowCreateInfo
	{
		int width{ 1280 };
		int height{ 720 };
		const char* title{ "Source" };
		bool isFullscreen{ false };
	};

	class WindowSystem
	{
	public:
		WindowSystem() = default;
		~WindowSystem();
		void Initialize(WindowCreateInfo createInfo);
		void PollEvents() const;
		bool ShouldClose() const;
		void SetTitle(const char* title);
		GLFWwindow* GetWindow() const;
		std::array<int, 2> GetWindowSize() const;

		typedef std::function<void()> OnResetFunc;
		typedef std::function<void(int, int, int, int)> OnKeyFunc;
		typedef std::function<void(unsigned int)> OnCharFunc;
		typedef std::function<void(int, unsigned int)> OnCharModsFunc;
		typedef std::function<void(int, int, int)> OnMouseButtonFunc;
		typedef std::function<void(double, double)> OnCursorPosFunc;
		typedef std::function<void(int)> OnCursorEnterFunc;
		typedef std::function<void(double, double)> OnScrollFunc;
		typedef std::function<void(int, const char**)> OnDropFunc;
		typedef std::function<void(int, int)> OnWindowSizeFunc;
		typedef std::function<void()> OnWindowCloseFunc;

		void RegisterOnResetFunc(OnResetFunc func) {}

		bool IsMouseButtonDown(int button) const
		{
			if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
			{
				return false;
			}
			return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
		}
	protected:
		// window event callbacks
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (const WindowSystem* app = static_cast<WindowSystem*>(glfwGetWindowUserPointer(window)))
			{
				app->on_key(key, scancode, action, mods);
			}
		}
		void on_key(const int key, int scancode, const int action, int mods) const
		{
			for (auto& func : m_on_key_func_)
				func(key, scancode, action, mods);
		}
	private:
		GLFWwindow* m_Window{ nullptr };
		int m_width_{ 0 };
		int m_height_{ 0 };
		std::vector<OnKeyFunc> m_on_key_func_;
	};
}
