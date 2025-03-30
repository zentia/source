#pragma once
#include <memory>

#include "Editor/base//SceneInspector.h"
#include "Runtime/Interface/IApplication.h"
#include "PlayerLoopController.h"
#include "Editor/input/editor_input_module.h"
#include "Editor/scene/editor_scene_manager.h"
#include "Modules/config/config_module.h"
#include "Modules/render/render_module.h"
#include "Modules/window/window_module.h"
#include "Modules/world/world_module.h"
#include "modules/ui/ui_module.h"

class application : public IApplication, public ISceneInsector
{
    double m_LaunchTime;

public:
    application();
    virtual ~application();

    void initialize(const std::string& config_file_path);

    virtual void RequestRecreateGfxDevice();
    virtual void RequestLoadRenderDoc();
    virtual void SetSceneRepaintDirty() {}
    virtual void ForceRepaintNextFrame() {}
    virtual bool IsPaused() const { return m_PlayerLoopController.IsPaused(); }
    std::shared_ptr<log_system>& GetLogSystem() { return m_LoggerSystem; }
    std::shared_ptr<source_runtime::window_module>& get_window_system() { return m_window_module; }
    void clear();
public:
    bool m_RecreateGfxDevice;
    bool m_LoadRenderDoc;
    PlayerLoopController m_PlayerLoopController;
    std::shared_ptr<log_system> m_LoggerSystem;
    std::shared_ptr<source_runtime::window_module> m_window_module;
    std::shared_ptr<source_runtime::config_module> m_config_module;
    std::shared_ptr<source_runtime::render_module> m_render_module;
    std::shared_ptr<source_runtime::world_module> m_world_module;
    std::shared_ptr<source_editor::editor_scene_manager> m_scene_manager{ nullptr };
    std::shared_ptr<source_editor::editor_input_module> m_input_module{ nullptr };
    std::shared_ptr<source_runtime::ui::ui_module> m_ui_module;
};

application& get_application();