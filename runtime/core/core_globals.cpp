#include "core_globals.h"

namespace source_runtime::core
{
	void initialize()
	{
		game_thread_id_initialized = true;
	}

	bool is_in_game_thread()
	{
		return true;
	}

	bool is_in_rendering_thread()
	{
		return true;
	}

}
	