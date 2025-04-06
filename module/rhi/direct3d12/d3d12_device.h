#pragma once
#include <memory>

#include "d3d12_queue.h"
#include "d3d12_rhi_common.h"
#include "d3d12_submission.h"

#include <array>

namespace source_module::rhi
{
	class d3d12_queue final
	{
	public:
		d3d12_device* m_device{ nullptr };
		d3d12_queue_type m_queue_type;
		std::shared_ptr<ID3D12CommandQueue> m_command_queue;
		d3d12_fence m_d3d12_fence;
	};

	class d3d12_device final : public d3d12_single_node_gpu_object, public d3d12_adapter_child
	{
	public:
		std::array<d3d12_queue, static_cast<uint32_t>(d3d12_queue_type::count)> m_queues;
	};

	
}
