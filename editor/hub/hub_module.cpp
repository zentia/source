#include "hub_module.h"

#include "hub_panel.h"
#include "editor/base/application/application.h"
#include "modules/ui/widgets/button/button.h"

namespace source_editor::hub
{
    hub_module::hub_module()
    {
        m_main_panel_ = std::make_unique<hub_panel>();
    }

    void hub_module::initialize(std::string&& project_path)
    {
        m_project_path_ = project_path;
    }

    void hub_module::run()
    {
        application* application = application::instance();
        const auto window_module = application->m_window_module;
        const auto render_module = application->m_render_module;
	    while (!window_module->should_close())
	    {
            application->update();
            source_module::window::window_module::poll_events();
            render_module->update();
		    if (!m_main_panel_->is_opened())
		    {
                window_module->set_should_close(true);
		    }

	    }
    }

}
