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
			application::instance()->m_window_module,
			application::instance()->m_render_module
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
		while (true)
		{
			const application* application = application::instance();
			application->m_time_module->update();
			application->m_scene_manager->update();
			application->m_input_module->update();
			
			if (!m_EngineRuntime->update())
				return;
		}
	}

}
