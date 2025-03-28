#include "SourcePrefix.h"
#include "application.h"

#include "Modules/render/render_module.h"
#include "Runtime/Allocator/MemoryManager.h"

static application* gApp = nullptr;

application::application()
	: m_RecreateGfxDevice(false)
	, m_LoadRenderDoc(false)
{
	gApp = this;
}

application::~application()
{
	gApp = nullptr;
}


void application::initialize_project(const std::string& config_file_path)
{
	MemoryManager::LateStaticInitialize();
	m_LoggerSystem = std::make_shared<log_system>();

	m_config_module = std::make_shared<source_runtime::config_module>();
	m_config_module->initialize(config_file_path);
	m_window_module = std::make_shared<source_runtime::window_module>();
	source_runtime::WindowCreateInfo windowCreateInfo;
	m_window_module->Initialize(windowCreateInfo);
	m_render_module = std::make_shared<source_runtime::render_module>();
	source_runtime::render_init_info render_init_info;
	render_init_info.window_system = m_window_module;
	m_render_module->initialize(render_init_info);

	m_world_module = std::make_shared<source_runtime::world_module>();
	m_world_module->initialize();

	m_input_module = std::make_shared<source_editor::editor_input_module>();
	m_input_module->initialize();
}

void application::RequestRecreateGfxDevice()
{
	m_RecreateGfxDevice = true;
}

void application::RequestLoadRenderDoc()
{
	m_LoadRenderDoc = true;
}

application& get_application()
{
	return *gApp;
}

void application::clear()
{
	
}
