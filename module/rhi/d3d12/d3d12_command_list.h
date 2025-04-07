#pragma once
#include <d3d12.h>
#include <memory>

namespace source_module::rhi
{
	class d3d12_command_list final
	{
	public:
	private:
		std::shared_ptr<ID3D12CommandList> m_command_list_;
		std::shared_ptr<ID3D12GraphicsCommandList> m_copy_command_list_;
		std::shared_ptr<ID3D12GraphicsCommandList> m_graphics_command_list_;

	};
}
