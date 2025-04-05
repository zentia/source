#include "editor.h"
#include "runtime/application/application.h"

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
	}

	void editor::Clear()
	{
		
	}

	void editor::Run()
	{
		assert(m_EngineRuntime);
		while (true)
		{
			const source_runtime::application* application = source_runtime::application::instance();
			application->m_time_module->update();
			application->m_scene_manager->update();
			application->m_input_module->update();
			
			if (!m_EngineRuntime->update())
				return;
		}
	}

}
