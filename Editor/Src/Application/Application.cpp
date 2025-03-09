#include "SourcePrefix.h"
#include "Application.h"

#include "Runtime/Allocator/MemoryManager.h"

static Application* gApp = nullptr;

Application::Application()
	: m_RecreateGfxDevice(false)
	, m_LoadRenderDoc(false)
{
	gApp = this;
}

Application::~Application()
{
	gApp = nullptr;
}


void Application::InitializeProject()
{
	MemoryManager::LateStaticInitialize();
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