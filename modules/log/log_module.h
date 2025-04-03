#pragma once
#include <memory>

#include "spdlog/spdlog.h"

namespace source_module::log
{
	class log_module final
	{
	public:
		log_module();
		~log_module();
	private:
		std::shared_ptr<spdlog::logger> m_logger_;
	};
}
