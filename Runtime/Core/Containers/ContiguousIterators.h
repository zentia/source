#pragma once

#include <iterator>

namespace core
{
	template<typename T>
	class const_contiguous_iterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef const value_type* pointer;
		typedef const value_type& reference;

		const_contiguous_iterator() : m_Ptr(nullptr) {}
		const_contiguous_iterator(pointer ptr) : m_Ptr(ptr) {}
		const_contiguous_iterator(const const_contiguous_iterator<T>& other) : m_Ptr(&(*other)) {}

		reference operator*() const
		{
			return *m_Ptr;
		}

		pointer operator->() const
		{
			return m_Ptr;
		}

		const_contiguous_iterator& operator++()
		{
			++m_Ptr;
			return *this;
		}

		const_contiguous_iterator operator++(int)
		{
			const_contiguous_iterator cur = *this;
			++*this;
			return cur;
		}

		bool operator==(const const_contiguous_iterator& rhs) const
		{
			return m_Ptr == rhs.m_Ptr;
		}

		bool operator!=(const const_contiguous_iterator& rhs) const
		{
			return (!(*this == rhs));
		}
	private:
		pointer m_Ptr;
	};
}
