#pragma once

namespace source_runtime::math
{
	template<typename Type>
	class vector2
	{
	public:
		Type x, y;
	};
	using vector2f = vector2<float>;
	using vector2i = vector2<int32_t>;
}
