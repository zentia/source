#include "hub_module.h"

#include "hub_panel.h"
#include "SourcePrefix.h"
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
        application& application = get_application();
	    while (!application.m_window_module->should_close())
	    {
		    if (!m_main_panel_->is_opened())
		    {
                application.m_window_module->set_should_close(true);
		    }
	    }
    }

}
