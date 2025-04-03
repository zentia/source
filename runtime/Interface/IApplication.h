#pragma once
#include "Runtime/Modules/ExportModuels.h"

class IApplication
{
public:
    virtual ~IApplication() {}

    virtual void SetSceneRepaintDirty() = 0;
    virtual void ForceRepaintNextFrame() = 0;

    virtual bool IsPaused() const = 0;

    virtual void RequestRecreateGfxDevice() = 0;
    virtual void RequestLoadRenderDoc() = 0;
};

EXPORT_COREMODULE IApplication* GetIApplication();
EXPORT_COREMODULE void SetIApplication(IApplication* appliation);