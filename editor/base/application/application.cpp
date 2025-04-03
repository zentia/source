#include "application.h"

#include "Editor/ui/editor_ui.h"
#include "Modules/render/render_module.h"

static application* gApp = nullptr;

application::application(const std::string&& exe_path)
	: m_RecreateGfxDevice(false)
	, m_LoadRenderDoc(false)
	//, m_context(exe_path)
{
	//m_device = m_context.create_default_device();
	//m_stream = m_device.create_stream(luisa::compute::StreamTag::GRAPHICS);
	gApp = this;
}

application::~application()
{
	gApp = nullptr;
}


void application::initialize(const std::string& config_file_path)
{
	m_config_module = std::make_shared<source_runtime::config_module>();
	m_config_module->initialize(config_file_path);
	m_window_module = std::make_shared<source_runtime::window_module>(/*m_device, m_stream*/);
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

	m_ui_module = std::make_shared<source_editor::editor_ui>();
	const source_runtime::ui::window_ui_init_info ui_init_info
	{
		m_window_module,
		m_render_module
	};
	m_ui_module->initialize(ui_init_info);
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