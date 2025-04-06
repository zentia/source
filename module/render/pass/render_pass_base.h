#pragma once

namespace source_runtime::render
{
	struct render_pass_init_info
	{
		
	};
	class render_pass_base
	{
	public:
		render_pass_base() = default;
		virtual ~render_pass_base() = default;
		render_pass_base(render_pass_base& render_pass_base) = delete;
		render_pass_base(render_pass_base&& render_pass_base) = delete;
		render_pass_base& operator=(render_pass_base& render_pass_base) = delete;
		render_pass_base& operator=(render_pass_base&& render_pass_base) = delete;
		
		virtual void initialize(const render_pass_init_info* init_info) = 0;
	};
}
