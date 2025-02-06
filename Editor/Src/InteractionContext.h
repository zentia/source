#pragma once

class InteractionContext
{

};

void SetGlobalInteractionContext(InteractionContext* interactionContext);
InteractionContext* GetGlobalInteractionContext();