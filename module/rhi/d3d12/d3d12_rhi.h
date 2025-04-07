#pragma once
#include "d3d12_adapter.h"
#include "module/rhi/rhi_module.h"

namespace source_module::rhi
{
	class d3d12_rhi final : public rhi_module
	{
	public:
		void initialize(rhi_init_info init_info) override;
		rhi_interface_type get_interface_type() const override;
	private:
		void find_adapter();
		std::vector<d3d12_adapter*> m_chosen_adapters_{};
	};
}
