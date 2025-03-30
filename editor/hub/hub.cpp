#include "hub.h"

#include "SourcePrefix.h"
#include "Editor/base/application/application.h"
#include "Modules/ui/widgets/button/button.h"

namespace source_editor
{
    hub::hub()
    {
    }

    void hub::initialize(std::string&& project_path)
    {
        m_project_path_ = project_path;
    }

    void hub::run()
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
