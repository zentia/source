#include "application.h"

#include "runtime/core/core_globals.h"
#include "editor/ui/editor_ui.h"
#include "module/render/render_module.h"
#include "module/rhi/d3d12/d3d12_rhi.h"

namespace source_runtime
{
	application* application::m_application_ = nullptr;

	application::application(const std::string&& exe_path)
		: m_RecreateGfxDevice(false)
		, m_LoadRenderDoc(false)
	{
		m_application_ = this;
	}

	application::~application()
	{
		m_application_ = nullptr;
	}


	void application::initialize(const std::string& config_file_path)
	{
		core::initialize();
		m_config_module = std::make_shared<config_module>();
		m_config_module->initialize(config_file_path);

		m_asset_module = std::make_shared<source_module::asset::asset_module>(m_config_module->get_root_folder());

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

		const source_module::rhi::rhi_init_info rhi_init_info{};
		m_rhi_module = std::make_shared<source_module::rhi::d3d12_rhi>();
		m_rhi_module->initialize(rhi_init_info);
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
		m_ui_module->update();
	}

	void application::clear()
	{

	}

	void application::begin_frame_render_thread()
	{
		
	}

	application* application::instance()
	{
		return m_application_;
	}
}
