#pragma once

#include "Runtime/Logging/LogAssert.h"

class ListElement
{
public:
	inline ListElement();
	ListElement* GetPrev() const { return m_Prev; }
	ListElement* GetNext() const { return m_Next; }

	inline bool IsInList() const;
	inline bool RemoveFromList();
	inline void InsertInList(ListElement* pos);
protected:
	inline void SetAsListRoot();
private:
	ListElement* m_Prev;
	ListElement* m_Next;

	template<class T> friend class List;
};

template<class T>
class List;

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

	List();
	void push_back(T& node) { node.InsertInList(&m_Root); }
	iterator begin() { return iterator(m_Root.m_Next); }
	iterator end() { return iterator(&m_Root); }
private:
	ListElement m_Root;
};

template <class T>
List<T>::List()
{
	m_Root.SetAsListRoot();
}


inline bool ListElement::IsInList() const
{
	return m_Prev != nullptr;
}

inline bool ListElement::RemoveFromList()
{
	if (!IsInList())
		return false;
	m_Prev->m_Next = m_Next;
	m_Next->m_Prev = m_Prev;
	m_Prev = nullptr;
	m_Next = nullptr;
	return true;
}


inline void ListElement::InsertInList(ListElement* pos)
{
	if (this == pos)
	{
		return;
	}
	if (IsInList())
		RemoveFromList();
	m_Prev = pos->m_Prev;
	m_Next = pos;
	m_Prev->m_Next = this;
	m_Next->m_Prev = this;
}

inline void ListElement::SetAsListRoot()
{
	m_Prev = m_Next = this;
}

inline ListElement::ListElement()
{
	m_Prev = nullptr;
	m_Next = nullptr;
}
