#pragma once
#include <vector>

namespace source_runtime::vector
{
	template<typename T>
	void remove_at_swap(std::vector<T>& vec, size_t index)
	{
		if (index < vec.size())
		{
			vec[index] = vec.back();
			vec.pop_back();
		}
	}
}
