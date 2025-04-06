#pragma once
#include "render_pass.h"
#include "module/ui/ui_module.h"

namespace source_runtime::render
{
	struct ui_pass_init_info : render_pass_init_info
	{
		source_module::rhi::rhi_render_pass* render_pass;
	};

	class ui_pass final : public source_module::render::render_pass
	{
	public:
		ui_pass(ui_pass& pass) = delete;
		ui_pass(ui_pass&& pass) = delete;
		ui_pass& operator=(ui_pass& pass) = delete;
		ui_pass& operator=(ui_pass&& pass) = delete;
		~ui_pass() override = default;
		void initialize(const render_pass_init_info* init_info) override;
		void draw() override;
	private:
		ui::ui_module* m_ui_module_;
	};
}
