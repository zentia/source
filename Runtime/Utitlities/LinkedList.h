#pragma once

#include "Runtime/Logging/LogAssert.h"

class ListElement
{
public:
	ListElement* GetPrev() const { return m_Prev; }
	ListElement* GetNext() const { return m_Next; }

	inline void 
private:
	ListElement* m_Prev;
	ListElement* m_Next;
};

template<class T>
class ListIterator
{
public:
	ListIterator(T* node = nullptr) : m_Node(node) {}

	ListIterator& operator++() { m_Node = m_Node->GetNext(); return *this; }
	ListIterator operator++(int) { ListIterator ret(*this); ++(*this); return ret; }

	ListIterator& operator--() { m_Node = m_Node->GetPrev(); return *this; }
	ListIterator operator--(int) { ListIterator ret(*this); --(*this); return ret; }

	T& operator*() const { return static_cast<T&>(*m_Node); }
	T* operator->() const { return static_cast<T*>(m_Node); }

	friend bool operator!=(const ListIterator& x, const ListIterator& y) { return x.m_Node != y.m_Node; }
	friend bool operator==(const ListIterator& x, const ListIterator& y) { return x.m_Node == y.m_Node; }
private:
	friend class List<T>;
	ListIterator(ListElement* node) : m_Node(node) {}
	ListElement* m_Node;
};

template<class T>
class List
{
public:
	typedef ListIterator<T> iterator;

	void push_back(T& node) { node.I}
	iterator begin() { return iterator(m_Root.m_Next); }
	iterator end() { return iterator(&m_Root); }
private:
	ListElement m_Root;
};
