#include "ui_pass.h"

#include "runtime/application/application.h"

namespace source_runtime::render
{
	void ui_pass::draw()
	{
		const application* application = application::instance();
		if (application->m_ui_module)
		{
			
		}
	}

	void ui_pass::initialize(const render_pass_init_info* init_info)
	{
		render_pass::initialize(nullptr);

	}

}
