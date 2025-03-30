#pragma once
#include "render_pass_base.h"

namespace source_runtime::render
{
	class render_pass : public render_pass_base
	{
	public:
		virtual void draw();
	};
}
