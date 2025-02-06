#include "SourcePrefix.h"
#include "Application.h"

static Application* gApp = nullptr;

Application::Application()
    : m_RecreateGfxDevice(false)
    , m_LoadRenderDoc(false)
{
    gApp = this;
}

void Application::InitializeProject()
{

}

void Application::RequestRecreateGfxDevice()
{
    m_RecreateGfxDevice = true;
}

void Application::RequestLoadRenderDoc()
{
    m_LoadRenderDoc = true;
}

Application& GetApplication()
{
    return *gApp;
}