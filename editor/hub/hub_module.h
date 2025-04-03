#pragma once
#include "hub_panel.h"
#include "Modules/ui/ui_module.h"
#include "Modules/ui/canvas/canvas.h"
#include "Modules/ui/panel/panel_window.h"

namespace source_editor::hub
{
    class hub_module
    {
    public:
        hub_module();
        void initialize(std::string&& project_path);
        void run();
    private:
        
        source_runtime::ui::canvas m_canvas_;
        std::unique_ptr<hub_panel> m_main_panel_;
        std::string m_project_path_;
        std::string m_project_name_;
    };
}
