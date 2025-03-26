#pragma once

#include <memory>

#include "Runtime/Engine.h"
#include "EditorUI.h"

namespace SourceEditor
{
	class SourceEditor
	{
		friend class EditorUI;

	public:
		SourceEditor();
		virtual ~SourceEditor();

		void Initialize(SourceRuntime::SourceEngine* engineRuntime);
		void Clear();

		void Run();
	protected:
		std::shared_ptr<EditorUI> m_EditorUI;
		SourceRuntime::SourceEngine* m_EngineRuntime{ nullptr };
	};
}
