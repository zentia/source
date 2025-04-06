#pragma once

namespace source_module::render
{
	enum class render_dependency_graph_pass_flags : uint8_t
	{
		/// <summary>
		/// �����޲ε�AddPass����
		/// </summary>
		none = 0,
		/// <summary>
		/// pass��ͼ�ιܵ���ʹ�ù�դ��
		/// </summary>
		raster = 1 << 0,
		/// <summary>
		/// pass��ͼ�ιܵ���ʹ��compute
		/// </summary>
		compute = 1 << 1,
		/// <summary>
		/// pass��һ������ܵ���ʹ��compute
		/// </summary>
		async_compute = 1 << 2,
		/// <summary>
		/// pass��ͼ�ιܵ���ʹ�ø�������
		/// </summary>
		copy = 1 << 3,
		/// <summary>
		/// �����ü��Ż�����������pass
		/// </summary>
		never_cull = 1 << 4,
		/// <summary>
		/// ����begin_render_pass/end_render_pass�������û�ȥ���ã�ֻ��Raster��ʱ��Ч��������Pass�ϲ���
		/// </summary>
		skip_render_pass = 1 << 5,
		// Pass����ԭʼ��RHI��Դ����Щ��Դ���ܱ�ע�ᵽrdg�У���������Դ�������ڵ�ǰ״̬���˱�־��ֹͼ�ε��ȿ�Խͨ���ķָ��ϰ����κβ�ֶ����ӳٵ�passִ��֮����Դ���ܲ�����passִ�й����иı�״̬��Ӱ�����ܵ����ϣ�������async_compute���
		untrack_access = 1 << 6,
		// passʹ�ø��������д���ݴ���Դ
		read_back = copy | never_cull,
		// ��־���룬��ʾ�ύ��pass��RHI��������
		command_mask = raster | compute | async_compute | copy,
		// ���ɴ��ݱ�־������ʹ�õı�־����
		scope_mask = never_cull | untrack_access,
	};

	enum class render_dependency_graph_buffer_flags : uint8_t
	{
		// �ޱ��
		none = 0,
		// �����ڶ�֡
		multi_frame = 1 << 0
	};

	enum class rdg_texture_flags : uint8_t
	{
		none = 0,
		multi_frame = 1 << 0,
		// ��ֹ�ڴ������Ͻ�ѹԪ����
		maintain_compression = 1 << 1,
	};

	enum class render_dependency_graph_unordered_access_view_flags : uint8_t
	{
		none = 0,
		// ��������
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
		// ����uav ����д������
		texture_uav,
		// ����srv ���ڶ�ȡ����
		texture_srv,
		buffer_uav,
		buffer_srv,
		max
	};

	enum class rdg_texture_meta_data_access : uint8_t
	{
		// ��ƽ��Ĭ��ѹ��ʹ��
		none,
		compressed_surface,
		depth,
		stencil,
		h_tile,
		f_mask,
		c_mask,
	};
}
