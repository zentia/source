#include "log_module.h"

#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace source_module::log
{
	log_module::log_module()
	{
		const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern("[%^%l%$] %v");

		const spdlog::sinks_init_list sink_list = { console_sink };

		spdlog::init_thread_pool(8192, 1);

		m_logger_ = std::make_shared<spdlog::async_logger>("source_logger", sink_list.begin(), sink_list.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		m_logger_->set_level(spdlog::level::trace);

		spdlog::register_logger(m_logger_);
	}

	log_module::~log_module()
	{
		m_logger_->flush();
		spdlog::drop_all();
	}

}
