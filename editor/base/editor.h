#pragma once

#include <memory>

#include "Runtime/engine/Engine.h"

namespace source_editor
{
	class editor
	{
		friend class editor_ui;

	public:
		editor();
		virtual ~editor();

		void initialize(source_runtime::source_engine* engineRuntime);
		void Clear();

		void Run();
	protected:
		source_runtime::source_engine* m_EngineRuntime{ nullptr };
	};
}
