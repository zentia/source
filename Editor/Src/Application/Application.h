#pragma once
#include <memory>

#include "Editor/Src/SceneInspector.h"
#include "Runtime/Interface/IApplication.h"
#include "PlayerLoopController.h"
#include "Modules/Config/ConfigModule.h"

namespace Source
{
	class WindowSystem;
}

class Application : public IApplication, public ISceneInsector
{
    double m_LaunchTime;

public:
    Application();
    virtual ~Application();

    void InitializeProject(const std::string& config_file_path);

    virtual void RequestRecreateGfxDevice();
    virtual void RequestLoadRenderDoc();
    virtual void SetSceneRepaintDirty(){}
    virtual void ForceRepaintNextFrame(){}
    virtual bool IsPaused() const { return m_PlayerLoopController.IsPaused(); }
    std::shared_ptr<LogSystem>& GetLogSystem() { return m_LoggerSystem; }
    std::shared_ptr<Source::WindowSystem>& get_window_system() { return m_WindowSystem; }
    void clear();
private:
    bool m_RecreateGfxDevice;
    bool m_LoadRenderDoc;
    PlayerLoopController m_PlayerLoopController;
    std::shared_ptr<LogSystem> m_LoggerSystem;
    std::shared_ptr<Source::WindowSystem> m_WindowSystem;
    std::shared_ptr<source_runtime::config_module> m_config_module_;
};

Application& GetApplication();