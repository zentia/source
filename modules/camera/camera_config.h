#pragma once
#include "Runtime/math/vector2.h"
#include "Runtime/math/vector3.h"

namespace source_runtime
{
	class camera_pose
	{
	public:
		math::vector3 m_position;
		math::vector3 m_target;
		math::vector3 m_up;
	};

	class camera_config
	{
	public:
		camera_pose m_pose;
		math::vector2 m_aspect;
		float m_z_far;
		float m_z_near;
	};
}
