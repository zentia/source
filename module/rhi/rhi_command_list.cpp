#include "rhi_command_list.h"

namespace source_module::rhi
{
	rhi_command_list_immediate& rhi_command_list_executor::get_immediate_command_list()
	{
		return g_rhi_command_list_executor.m_command_list_immediate;
	}

}
