#pragma once

#include <cctype>

#include "Runtime/Utitlities/TypeUtilities.h"

enum ComparisonType
{
	kComparisonCaseSensitive,
	kComparisonIgnoreCase
};

namespace core
{
	template<typename TChar>
	class basic_string_operations
	{
	public:
		typedef TChar value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		typedef typename Conditional<sizeof(value_type) == 1, unsigned char,
			typename Conditional<sizeof(value_type) == 2, unsigned short,
			typename Conditional<sizeof(value_type) == 4, unsigned int, NullType>::type>::type>::type unsigned_value_type;

		FORCE_INLINE static int compare_no_check(const value_type* str, size_type len, const value_type* compareStr, ComparisonType comparisonType = kComparisonCaseSensitive)
		{
			const value_type* last = str + len;

			if (comparisonType == kComparisonIgnoreCase)
			{

			}

			return -static_cast<unsigned_value_type>(*compareStr);
		}
	};
}