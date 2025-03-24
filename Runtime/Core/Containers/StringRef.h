#pragma once
#include "ContiguousIterators.h"
#include "Runtime/Core/Containers/StringOperations.h"
#include "Runtime/Utitlities/TypeUtilities.h"

namespace core
{
	template<typename TChar>
	class basic_string_ref : private basic_string_operations<TChar>
	{
	public:
		typedef basic_string_operations<TChar> operations_type;
		typedef typename operations_type::value_type value_type;
		typedef typename operations_type::size_type size_type;
		typedef typename operations_type::const_pointer const_pointer;
		typedef typename core::const_contiguous_iterator<TChar> iterator;
		typedef typename core::const_contiguous_iterator<TChar> const_iterator;

		const value_type* m_data;

		size_type m_size;

		basic_string_ref()
			: m_data(nullptr)
			, m_size(0) {}

		template<class T>
		SOURCE_FORCEINLINE basic_string_ref(const T& str, typename EnableIf<string_traits<T>::is_supported>::type* = 0)
		{
			assign(string_traits<T>::get_data(str), string_traits<T>::get_size(str));
		}

		basic_string_ref(const value_type* refStr, size_type refStrLen)
		{
			assign(refStr, refStrLen);
		}

		basic_string_ref& assign(const value_type* refStr, size_type refStrLen)
		{
			m_data = refStr;
			m_size = refStrLen;
			return *this;
		}

		template<class T>
		SOURCE_FORCEINLINE basic_string_ref& assign(typename EnableIf<string_traits<T>::is_supported, const T>::type& refStr)
		{
			return assign(string_traits<T>::get_data(refStr), string_traits<T>::get_size(refStr));
		}

		template<class T>
		SOURCE_FORCEINLINE basic_string_ref& operator=(typename EnableIf<string_traits<T>::is_supported, const T>::type& refStr)
		{
			return assign(string_traits<T>::get_data(refStr), string_traits<T>::get_size(refStr));
		}

		basic_string_ref& operator=(const value_type* refStr)
		{
			return assign(refStr, string_traits<const_pointer>::get_size(refStr));
		}

		template<class T>
		int compare(const T& compareStr, ComparisonType comparisonType = kComparisonCaseSensitive, typename EnableIf<string_traits<T>::is_supported>::type* = 0) const
		{
			return compare_no_check(data(), length(), string_traits<T>::get_data(compareStr), string_traits<T>::get_size(compareStr), comparisonType);
		}

		int compare(const value_type* compareStr, ComparisonType comparisonType = kComparisonCaseSensitive) const
		{
			return basic_string_operations<TChar>::compare_no_check(data(), size(), compareStr, comparisonType);
		}

		const value_type* data() const
		{
			return m_data;
		}

		size_type length() const
		{
			return m_size;
		}

		size_type size() const
		{
			return m_size;
		}

		iterator begin()
		{
			return iterator(data());
		}

		iterator end()
		{
			return iterator(data() + length());
		}
	};

	typedef basic_string_ref<char> string_ref;
	typedef basic_string_ref<wchar_t> wstring_ref;
}
