#include "Editor/Src/InteractionContext.h"

namespace 
{
    InteractionContext* gGlobalInteractionContext = nullptr;
}

void SetGlobalInteractionContext(InteractionContext* interactionContext)
{
    gGlobalInteractionContext = interactionContext;
}

InteractionContext* GetGlobalInteractionContext()
{
    return gGlobalInteractionContext;
}