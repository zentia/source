#include "hub_module.h"

#include "hub_panel.h"
#include "Editor/base/application/application.h"
#include "Modules/ui/widgets/button/button.h"

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
        const auto window_module = get_application().m_window_module;
	    while (!window_module->should_close())
	    {
            //window_module->prepare_frame();
            //window_module->render_frame();
		    if (!m_main_panel_->is_opened())
		    {
                window_module->set_should_close(true);
		    }

	    }
    }

}
