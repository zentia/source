#include "core_globals.h"
#include "windows_platform_tls.h"

namespace source_runtime::core
{
	void initialize()
	{
		game_thread_id = platform_tls::get_current_thread_id();
		game_thread_id_initialized = true;
	}

	bool is_in_game_thread()
	{
		if (game_thread_id_initialized)
		{
			return game_thread_id == platform_tls::get_current_thread_id();
		}
		return true;
	}
}
	