#include "application.h"

#include "editor/ui/editor_ui.h"
#include "modules/render/render_module.h"

application* application::m_application_ = nullptr;

application::application(const std::string&& exe_path)
	: m_RecreateGfxDevice(false)
	, m_LoadRenderDoc(false)
	//, m_context(exe_path)
{
	//m_device = m_context.create_default_device();
	//m_stream = m_device.create_stream(luisa::compute::StreamTag::GRAPHICS);
	m_application_ = this;
}

application::~application()
{
	m_application_ = nullptr;
}


void application::initialize(const std::string& config_file_path)
{
	m_config_module = std::make_shared<source_runtime::config_module>();
	m_config_module->initialize(config_file_path);
	m_window_module = std::make_shared<source_module::window::window_module>(/*m_device, m_stream*/);
	source_module::window::window_create_info windowCreateInfo;
	m_window_module->initialize(windowCreateInfo);
	m_render_module = std::make_shared<source_module::render::render_module>();
	source_module::render::render_init_info render_init_info;
	render_init_info.window_module = m_window_module;
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

	m_time_module = std::make_shared<source_module::time::time_module>();
	m_time_module->initialize();
}

void application::RequestRecreateGfxDevice()
{
	m_RecreateGfxDevice = true;
}

void application::RequestLoadRenderDoc()
{
	m_LoadRenderDoc = true;
}

void application::update() const
{
	m_time_module->update();
}

void application::clear()
{

}

application* application::instance()
{
	return m_application_;
}