#pragma once
#include "Runtime/Math/Vector2.h"
#include "Runtime/Math/vector3.h"

namespace source_runtime
{
	class camera_pose
	{
	public:
		vector3 m_position;
		vector3 m_target;
		vector3 m_up;
	};

	class camera_config
	{
	public:
		camera_pose m_pose;
		Vector2f m_aspect;
		float m_z_far;
		float m_z_near;
	};
}
