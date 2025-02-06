#pragma once

class InteractionContext;

int DisplayDialogComplex(const core::string& title, const core::string& content, const core::string& okButton, const core::string& secondary, const core::string& third);
int GetDialogResponseComplex(InteractionContext* interactionContext, const core::string& title, const core::string& content, const core::string& okButton, const core::string& secondary, const core::string& third);