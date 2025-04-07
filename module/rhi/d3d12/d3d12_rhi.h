#pragma once
#include "d3d12_adapter.h"
#include "d3d12_command_context.h"
#include "module/rhi/rhi_module.h"

namespace source_module::rhi
{
	class d3d12_rhi final : public rhi_module
	{
	public:
		void initialize(rhi_init_info init_info) override;
		rhi_interface_type get_interface_type() const override;
		std::shared_ptr<rhi_viewport> create_viewport(void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format pixel_format) override;
		virtual d3d12_command_context* create_command_context();
		ID3D12GraphicsCommandList* m_graphics_command_list{ nullptr };
	private:
		void find_adapter();
		std::vector<d3d12_adapter*> m_chosen_adapters_{};
	};
}
