#include "editor.h"
#include "application/application.h"
#include "editor/ui/editor_ui.h"

namespace source_editor
{
	editor::editor()
	{
		
	}

	editor::~editor()
	{
		
	}

	void editor::initialize(source_runtime::source_engine* engineRuntime)
	{
		assert(engineRuntime);

		m_EngineRuntime = engineRuntime;
		m_editor_ui_ = std::make_shared<editor_ui>();
		source_runtime::ui::window_ui_init_info ui_init_info = 
		{
			get_application().m_window_module,
			get_application().m_render_module
		};
		m_editor_ui_->initialize(ui_init_info);
	}

	void editor::Clear()
	{
		
	}

	void editor::Run()
	{
		assert(m_EngineRuntime);
		assert(m_editor_ui_);
		float delta_time;
		while (true)
		{
			delta_time = m_EngineRuntime->calculate_delta_time();
			application& application = get_application();
			application.m_scene_manager->tick(delta_time);
			application.m_input_module->tick(delta_time);
			if (!m_EngineRuntime->tick_one_frame(delta_time))
				return;
		}
	}

}
