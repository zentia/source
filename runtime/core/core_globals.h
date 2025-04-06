#pragma once
#include <cstdint>

namespace source_runtime::core
{
	inline uint32_t game_thread_id{0};
	inline bool game_thread_id_initialized{ false };
	void initialize();
	bool is_in_game_thread();
}
