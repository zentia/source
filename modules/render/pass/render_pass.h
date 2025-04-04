#pragma once
#include <vector>

#include "render_pass_base.h"
#include "Modules/render/pipeline/render_pipeline_base.h"
#include "Modules/rhi/rhi_struct.h"

namespace source_module::render
{
	class render_pass : public source_runtime::render::render_pass_base
	{
	public:
		struct frame_buffer_attachment
		{
			rhi::rhi_image* image;
			rhi::rhi_device_memory* memory;
			rhi::rhi_image_view* view;
			rhi_format format;
		};
		struct frame_buffer
		{
			int width;
			int height;
			rhi::rhi_frame_buffer* buffer;
			rhi::rhi_render_pass pass;

		};
		virtual void draw();
		void initialize(const source_runtime::render::render_pass_init_info* init_info) override;
		std::vector<render_pipeline_base> m_render_pipeline_bases;
		frame_buffer m_frame_buffer;
	};
}
