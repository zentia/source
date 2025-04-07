#pragma once

namespace source_runtime::math
{
	template<typename Type>
	class vector3
	{
	public:
		Type x, y, z;
	};
	using vector3f = vector3<float>;
	using vector3i = vector3<int32_t>;
}
