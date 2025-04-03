#pragma once
#include "render_pass.h"

namespace source_runtime::render
{
	class ui_pass : public render_pass
	{
	public:
		void draw() override;
	private:
	};
}
