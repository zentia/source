#pragma once

#include <string>

#include "Runtime/Core/Containers/StringStorageDefault.h"
#include "Runtime/Core/Containers/StringOperations.h"

namespace core
{
	template<typename TChar>
	class basic_string_ref;

	template<typename TChar, typename TStorage = StringStorageDefault<TChar> >
	class basic_string : private TStorage, private basic_string_operations<typename TStorage::value_type>
	{
	public:
		typedef basic_string_operations<TChar> operations_type;
		typedef typename operations_type::value_type value_type;
		typedef typename operations_type::size_type size_type;
		typedef typename operations_type::difference_type difference_type;
		typedef typename operations_type::pointer pointer;
		typedef typename operations_type::const_pointer const_pointer;
		typedef typename operations_type::reference reference;
		typedef typename operations_type::const_reference const_reference;

		basic_string(MemLabelRef label)
			: TStorage(label)
		{}

		SOURCE_FORCEINLINE basic_string(const value_type* copyStr)
			: TStorage(kMemString)
		{
			TStorage::assign(copyStr, std::char_traits<value_type>::length(copyStr));
		}

		SOURCE_FORCEINLINE basic_string(const value_type* copyStr, MemLabelId label)
			: TStorage(label)
		{
			TStorage::assign(copyStr, std::char_traits<value_type>::length(copyStr));
		}

		size_type length() const
		{
			return TStorage::size();
		}

		bool empty() const
		{
			return length() == 0;
		}

		const value_type* c_str() const
		{
			return TStorage::get_data();
		}

		SOURCE_FORCEINLINE basic_string& assign_external(const value_type* referenceStr)
		{
			TStorage::assign_external(const_cast<value_type*>(referenceStr), std::char_traits<value_type>::length(referenceStr));
			return *this;
		}

		SOURCE_FORCEINLINE static basic_string create_from_external(const value_type* referenceStr, MemLabelRef label = kMemString)
		{
			basic_string s(label);
			s.assign_external(const_cast<value_type*>(referenceStr));
			return s;
		}
	};

	typedef basic_string<char> string;
	typedef basic_string<wchar_t> wstring;
}