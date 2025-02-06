#pragma once
#include "SourcePrefix.h"

class LocalizationDatabase
{
public:
	static LocalizationDatabase& GetEditorDatabase();

	static void CreateInstance();
};
