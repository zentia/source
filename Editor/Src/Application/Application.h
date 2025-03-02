#pragma once
#include "Editor/Src/SceneInspector.h"
#include "Runtime/Interface/IApplication.h"
#include "PlayerLoopController.h"

class Application : public IApplication, public ISceneInsector
{
    double m_LaunchTime;

public:
    Application();
    virtual ~Application();

    void InitializeProject();

    virtual void RequestRecreateGfxDevice();
    virtual void RequestLoadRenderDoc();
    virtual void SetSceneRepaintDirty(){}
    virtual void ForceRepaintNextFrame(){}
    virtual bool IsPaused() const { return m_PlayerLoopController.IsPaused(); }
private:
    bool m_RecreateGfxDevice;
    bool m_LoadRenderDoc;
    PlayerLoopController m_PlayerLoopController;
};

Application& GetApplication();