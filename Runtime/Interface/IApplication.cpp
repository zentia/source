#include "IApplication.h"

static IApplication* gApplication = nullptr;

IApplication* GetIApplication()
{
    return gApplication;
}

void SetIApplication(IApplication* application)
{
    gApplication = application;
}