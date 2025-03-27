#include "SourcePrefix.h"
#include "Application.h"

#include "Modules/render/render_module.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/RenderCore/WindowSystem.h"

static Application* gApp = nullptr;

Application::Application()
	: m_RecreateGfxDevice(false)
	, m_LoadRenderDoc(false)
{
	gApp = this;
}

Application::~Application()
{
	gApp = nullptr;
}


void Application::InitializeProject(const std::string& config_file_path)
{
	MemoryManager::LateStaticInitialize();
	m_config_module = std::make_shared<source_runtime::config_module>();
	m_config_module->initialize(config_file_path);
	m_LoggerSystem = std::make_shared<LogSystem>();
	m_WindowSystem = std::make_shared<Source::WindowSystem>();
	Source::WindowCreateInfo windowCreateInfo;
	m_WindowSystem->Initialize(windowCreateInfo);
	m_render_module = std::make_shared<source_runtime::render_module>();
	source_runtime::render_init_info render_init_info;
	render_init_info.window_system = m_WindowSystem;
	m_render_module->initialize(render_init_info);
}

void Application::RequestRecreateGfxDevice()
{
	m_RecreateGfxDevice = true;
}

void Application::RequestLoadRenderDoc()
{
	m_LoadRenderDoc = true;
}

Application& GetApplication()
{
	return *gApp;
}

void Application::clear()
{
	
}
