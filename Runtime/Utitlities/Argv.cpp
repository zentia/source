#include "SourcePrefix.h"
#include "Argv.h"

static int argc;
static const char** argv;

bool HasARGV(core::string_ref name)
{
	for (int i = 0; i < argc; i++)
	{
		int j = 0;
		if (argv[i][j++] == '-')
		{
			if (argv[i][j] == '-') j++;

			if (StrIEquals(name, &argv[i][j]))
				return true;
		}
	}
	return false;
}