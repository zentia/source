#pragma once
#include <mutex>

#include "render_thread.h"
#include "module/rhi/rhi_command_list.h"
#include "runtime/util/vector_extension.h"

namespace source_runtime::render_core
{
	class render_resource
	{
	public:
		enum class init_phase : uint8_t
		{
			pre,
			def
		};
		static void init_pre_rhi_resources();
		virtual void init_rhi(source_module::rhi::rhi_command_list_base& rhi_cmd_list){}
		virtual void release_rhi(){}
		virtual void init_resource(source_module::rhi::rhi_command_list_base& rhi_cmd_list);
	private:
		int32_t m_list_index_{ -1 };
		init_phase m_phase_ {init_phase::def};
	};

	class render_resource_list
	{
	public:
		template <render_resource::init_phase>
		static render_resource_list& get()
		{
			static render_resource_list instance;
			return instance;
		}

		static render_resource_list& get(const render_resource::init_phase init_phase)
		{
			if (init_phase == render_resource::init_phase::pre)
				return get<render_resource::init_phase::pre>();
			return get<render_resource::init_phase::def>();
		}

		int32_t allocate(render_resource* resource)
		{
			m_recursive_mutex_.lock();
			int32_t index;
			if (m_free_index_list_.empty())
			{
				index = static_cast<int32_t>(m_resource_list_.size());
				m_resource_list_.emplace_back(resource);
			}
			else
			{
				index = vector::pop(m_free_index_list_);
				m_resource_list_[index] = resource;
			}
			m_recursive_mutex_.unlock();
			return index;
		}
	private:
		std::recursive_mutex m_recursive_mutex_;
		std::vector<int32_t> m_free_index_list_;
		std::vector<render_resource*> m_resource_list_;
	};
	void begin_init_resource(render_resource* resource, render_command_pipe* render_command_pipe = nullptr);
}
