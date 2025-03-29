#include "SourcePrefix.h"

#include "Editor/base//DisplayDialog.h"
#include "Editor/base//InteractionContext.h"

int DisplayDialogComplexPlatformSpecific(const core::string& title, const core::string& content, const core::string& okButton, const core::string& secondary, const core::string& third);

int DisplayDialogComplex(const core::string& title, const core::string& content, const core::string& okButton, const core::string& secondary, const core::string& third)
{
	return GetDialogResponseComplex(GetGlobalInteractionContext(), title, content, okButton, secondary, third);
}

int GetDialogResponseComplex(InteractionContext* interactionContext, const core::string& title, const core::string& content, const core::string& okButton, const core::string& secondary, const core::string& third)
{
	return DisplayDialogComplexPlatformSpecific(title, content, okButton, secondary, third);
}