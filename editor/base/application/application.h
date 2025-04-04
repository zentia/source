#pragma once
#include <memory>

#include "Runtime/Interface/IApplication.h"
#include "PlayerLoopController.h"
#include "editor/input/editor_input_module.h"
#include "editor/scene/editor_scene_manager.h"
#include "modules/config/config_module.h"
#include "modules/render/render_module.h"
#include "modules/time/time_module.h"
#include "modules/window/window_module.h"
#include "modules/world/world_module.h"
#include "modules/ui/ui_module.h"
//#include "luisa/runtime/context.h"
//#include "luisa/runtime/stream.h"

class application : public IApplication
{
    double m_LaunchTime;

public:
    application(const std::string&& exe_path);
    virtual ~application();

    void initialize(const std::string& config_file_path);

    virtual void RequestRecreateGfxDevice();
    virtual void RequestLoadRenderDoc();
    virtual void SetSceneRepaintDirty() {}
    virtual void ForceRepaintNextFrame() {}
    virtual bool IsPaused() const { return m_PlayerLoopController.IsPaused(); }
    std::shared_ptr<source_module::window::window_module>& get_window_system() { return m_window_module; }
    void update() const;
    void clear();
    
    bool m_RecreateGfxDevice;
    bool m_LoadRenderDoc;
    PlayerLoopController m_PlayerLoopController;
    std::shared_ptr<source_module::window::window_module> m_window_module;
    std::shared_ptr<source_runtime::config_module> m_config_module;
    std::shared_ptr<source_module::render::render_module> m_render_module;
    std::shared_ptr<source_runtime::world_module> m_world_module;
    std::shared_ptr<source_editor::editor_scene_manager> m_scene_manager{ nullptr };
    std::shared_ptr<source_editor::editor_input_module> m_input_module{ nullptr };
    std::shared_ptr<source_runtime::ui::ui_module> m_ui_module;
    std::shared_ptr<source_module::time::time_module> m_time_module{ nullptr };
    /*luisa::compute::Context m_context;
    luisa::compute::Device m_device;
    luisa::compute::Stream m_stream;*/
    static application* instance();
private:
    static application* m_application_;
};