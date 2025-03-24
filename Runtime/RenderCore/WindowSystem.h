#pragma once

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
		
	};
}
