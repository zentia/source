#pragma once

namespace source_module::render
{
	enum class render_dependency_graph_pass_flags : uint8_t
	{
		/// <summary>
		/// 用于无参的AddPass函数
		/// </summary>
		none = 0,
		/// <summary>
		/// pass在图形管道上使用光栅化
		/// </summary>
		raster = 1 << 0,
		/// <summary>
		/// pass在图形管道上使用compute
		/// </summary>
		compute = 1 << 1,
		/// <summary>
		/// pass在一部计算管道上使用compute
		/// </summary>
		async_compute = 1 << 2,
		/// <summary>
		/// pass在图形管道上使用复制命令
		/// </summary>
		copy = 1 << 3,
		/// <summary>
		/// 不被裁剪优化，用于特殊pass
		/// </summary>
		never_cull = 1 << 4,
		/// <summary>
		/// 忽略begin_render_pass/end_render_pass，留给用户去调用，只在Raster绑定时有效，将禁用Pass合并。
		/// </summary>
		skip_render_pass = 1 << 5,
		// Pass访问原始的RHI资源，这些资源可能被注册到rdg中，但所有资源都保持在当前状态，此标志阻止图形调度跨越通道的分割障碍。任何拆分都被延迟到pass执行之后。资源可能不会在pass执行过程中改变状态。影响性能的屏障，不能与async_compute组合
		untrack_access = 1 << 6,
		// pass使用复制命令，但写入暂存资源
		read_back = copy | never_cull,
		// 标志掩码，表示提交给pass的RHI命令类型
		command_mask = raster | compute | async_compute | copy,
		// 可由传递标志作用域使用的标志掩码
		scope_mask = never_cull | untrack_access,
	};

	enum class render_dependency_graph_buffer_flags : uint8_t
	{
		// 无标记
		none = 0,
		// 存续于多帧
		multi_frame = 1 << 0
	};

	enum class rdg_texture_flags : uint8_t
	{
		none = 0,
		multi_frame = 1 << 0,
		// 防止在此纹理上解压元数据
		maintain_compression = 1 << 1,
	};

	enum class render_dependency_graph_unordered_access_view_flags : uint8_t
	{
		none = 0,
		// 忽略屏障
		skip_barrier = 1 << 0,
	};

	enum class rdg_parent_resource_type : uint8_t
	{
		texture,
		buffer,
		max,
	};

	enum class rdg_view_type : uint8_t
	{
		// 纹理uav 用于写入数据
		texture_uav,
		// 纹理srv 用于读取数据
		texture_srv,
		buffer_uav,
		buffer_srv,
		max
	};

	enum class rdg_texture_meta_data_access : uint8_t
	{
		// 主平面默认压缩使用
		none,
		compressed_surface,
		depth,
		stencil,
		h_tile,
		f_mask,
		c_mask,
	};
}
