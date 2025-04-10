#pragma once
#include "EASTL/vector.h"

namespace source_runtime::enum_range
{
	template <typename EnumType>
	struct iterator
	{
		using int_type = eastl::underlying_type_t<EnumType>;

		explicit iterator(EnumType flags) :m_flags_(int_type(flags))
		{
			
		}
		iterator& operator++()
		{
			const int_type popped_bit = m_flags_ & (~m_flags_ + 1);
			m_flags_ ^= popped_bit;
			return *this;
		}
		EnumType operator*() const
		{
			const int_type result = m_flags_ & (~m_flags_ + 1);
			return static_cast<EnumType>(result);
		}
	private:
		int_type m_flags_;

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			return lhs.m_flags_ != rhs.m_flags_;
		}
	};

	template <typename EnumType>
	struct range
	{
		explicit range(EnumType flags) : m_flags_(flags){}
		iterator<EnumType> begin() const { return iterator<EnumType>(m_flags_); }
		iterator<EnumType> end() const { return iterator<EnumType>(EnumType(0)); }
	private:
		EnumType m_flags_;
	};

	template <typename EnumType>
	range<EnumType> make_flags_range(EnumType flags)
	{
		return range<EnumType>(flags);
	}
}
