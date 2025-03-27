#pragma once

namespace source_runtime
{
	struct rhi_offset_2d
	{
		int32_t x;
		int32_t y;
	};
	struct rhi_extent_2d
	{
		uint32_t width;
		uint32_t height;
	};
	struct rhi_rect_2d
	{
		rhi_offset_2d offset;
		rhi_extent_2d extend;
	};
	struct rhi_viewport
	{
		float x;
		float y;
		float width;
		float height;
		float min_depth;
		float max_depth;
	};
}
