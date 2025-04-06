#pragma once
#include "render_graph_scope.h"

namespace source_module::render
{
	class render_graph_builder : public render_graph_scope
	{
	public:
		void add_pass();
		void compile();
		void execute();
	};
}
