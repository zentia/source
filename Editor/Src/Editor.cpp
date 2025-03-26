#include "Editor.h"
#include "EditorUI.h"
#include "SourcePrefix.h"

namespace SourceEditor
{
	SourceEditor::SourceEditor()
	{
		
	}

	SourceEditor::~SourceEditor()
	{
		
	}

	void SourceEditor::Initialize(SourceRuntime::SourceEngine* engineRuntime)
	{
		assert(engineRuntime);

		m_EngineRuntime = engineRuntime;
		m_EditorUI = std::make_shared<EditorUI>();
	}

	void SourceEditor::Clear()
	{
		
	}

	void SourceEditor::Run()
	{
		assert(m_EngineRuntime);
		assert(m_EditorUI);
		float delta_time;
		while (true)
		{
			delta_time = m_EngineRuntime->calculate_delta_time();
			if (!m_EngineRuntime->tick_one_frame(delta_time))
				return;
		}
	}

}