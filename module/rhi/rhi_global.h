#pragma once

namespace source_module::rhi
{
	class rhi_global
	{
	public:
		struct shader_bundles
		{
			bool m_supports_parallel{ false };
		};
		shader_bundles m_shader_bundles;
	};

	inline rhi_global g_rhi_global{};
}
