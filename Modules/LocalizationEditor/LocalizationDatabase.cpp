#include "SourcePrefix.h"

#include "LocalizationDatabase.h"

#include "Runtime/Threads/Mutex.h"

static LocalizationDatabase* volatile s_pEditorInstance = nullptr;
static Mutex s_pEditorInstanceMutex;

LocalizationDatabase& LocalizationDatabase::GetEditorDatabase()
{
	if (!s_pEditorInstance)
	{
		Mutex::AutoLock lock(s_pEditorInstanceMutex);
		if (!s_pEditorInstance)
			CreateInstance();
	}
	return *s_pEditorInstance;
}


void LocalizationDatabase::CreateInstance()
{
	if (s_pEditorInstance)
		return;

}