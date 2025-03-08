#pragma once
#include <string>
#include "Runtime/Utitlities/Annotations.h"
#include "Runtime/Utitlities/TypeUtilities.h"

template<class T>
struct string_traits
{
	enum { is_null_terminateed = false, is_supported = false };
};

template<typename TChar, typename TTraits, typename TAlloc>
struct string_traits<std::basic_string<TChar, TTraits, TAlloc> >
{
	enum { is_null_terminated = true, is_supported = true };

	typedef TChar value_type;
	typedef std::basic_string<TChar, TTraits, TAlloc> type;

	static const value_type* get_data(const type& a) { return a.c_str(); }
	static typename type::size_type get_size(const type& a) { return a.size(); }
};

template<>
struct string_traits<const char*>
{
	enum {is_null_terminated = true, is_supported = true };
	typedef char value_type;
	SOURCE_FORCEINLINE static const value_type* get_data(const char* a) { return a; }
	SOURCE_FORCEINLINE static size_t get_size(const char* a) { return std::char_traits<char>::length(a); }
	static size_t calculate_size(const char* a, size_t max_length)
	{
		for (size_t i = 0; i < max_length; ++i)
		{
			if (a[i] == '\0') return i;
		}
		return max_length;
	}
};

template<size_t I>
struct string_traits<char[I]>
{
	enum { is_null_terminated = false, is_supported = true };

	typedef char value_type;

	static const value_type* get_data(const char(&a)[I]) { return a; }
	static size_t                   get_size(const char(&a)[I]) { return string_traits<const char*>::calculate_size(a, I); }
};

template<size_t I>
struct string_traits<const char[I]>
{
	enum { is_null_terminated = false, is_supported = true };
	typedef char value_type;
	static const value_type* get_data(const char(&a)[I]) { return a; }
	static size_t get_size(const char(&a)[I]) { return string_traits<const char*>::calculate_size(a, I); }
};

namespace StringTraits
{
	template<typename TChar, typename TStorage>
	inline const TChar* AsConstTChars(const core::basic_string<TChar, TStorage>& value)
	{
		return value.c_str();
	}

	template<typename TChar, typename TTraits, typename TAlloc>
	inline const TChar* AsConstTChars(const std::basic_string<TChar, TTraits, TAlloc>& value)
	{
		return value.c_str();
	}

	template<typename TChar>
	inline const TChar* AsConstTChars(const TChar* value)
	{
		return value;
	}
}
