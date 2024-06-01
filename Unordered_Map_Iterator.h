#pragma once
#include <forward_list>

template <typename Key,
	typename T,
	class Hash,
	size_t TableSize>
class Unordered_Map;

template <typename Unordered_Map>
class Unordered_Map_Iterator
{
private:
	using List = typename Unordered_Map::List;
	using Iterator = typename List::iterator;
	using value_type = typename List::value_type;

	Iterator m_iterator;
public:
	Unordered_Map_Iterator()
		: m_iterator(Iterator()) {}
	Unordered_Map_Iterator(Iterator iterator)
		: m_iterator(iterator) {}
	~Unordered_Map_Iterator() {}

	Iterator& bucketIterator() {
		return m_iterator;
	}

	Unordered_Map_Iterator& operator++() {
		m_iterator++;
		return (*this);
	}
	Unordered_Map_Iterator operator++(int) {
		Unordered_Map_Iterator temp = *this;
		++(*this);
		return temp;
	}
	Unordered_Map_Iterator& operator--() {
		m_iterator--;
		return (*this);
	}
	Unordered_Map_Iterator operator--(int) {
		Unordered_Map_Iterator temp = *this;
		--(*this);
		return temp;
	}

	typename List::value_type& operator*() const {
		return *m_iterator;
	}
	typename List::value_type* operator->() const {
		return &(*m_iterator);
	}

	friend bool operator!=(const Unordered_Map_Iterator& lhs, const Unordered_Map_Iterator& rhs) {
		return lhs.m_iterator != rhs.m_iterator;
	}
	friend bool operator==(const Unordered_Map_Iterator& lhs, const Unordered_Map_Iterator& rhs) {
		return lhs.m_iterator == rhs.m_iterator;
	}
	
};
