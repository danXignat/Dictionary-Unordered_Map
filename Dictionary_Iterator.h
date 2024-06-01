#pragma once
#include <vector>

template <typename Key,
	typename T,
	class Hash,
	size_t TableSize>
class Dictionary;

template <typename Dictionary>
class Dictionary_Iterator
{
private:
	using Vector = typename Dictionary::Vector;
	using Iterator = typename Vector::iterator;

	Iterator m_iterator;
public:
	Dictionary_Iterator()
		: m_iterator(Iterator()) {}
	Dictionary_Iterator(Iterator iterator)
		: m_iterator(iterator) {}
	~Dictionary_Iterator() {}

	Iterator& bucketIterator() {
		return m_iterator;
	}

	Dictionary_Iterator& operator++() {
		m_iterator++;
		return (*this);
	}
	Dictionary_Iterator operator++(int) {
		Dictionary_Iterator temp = *this;
		++(*this);
		return temp;
	}
	Dictionary_Iterator& operator--() {
		m_iterator--;
		return (*this);
	}
	Dictionary_Iterator operator--(int) {
		Dictionary_Iterator temp = *this;
		--(*this);
		return temp;
	}
	typename Dictionary::Pair_Type& operator*() const {
		return m_iterator->pair;
	}
	typename Dictionary::Pair_Type* operator->() const {
		return &(m_iterator->pair);
	}
	friend bool operator!=(const Dictionary_Iterator& lhs, const Dictionary_Iterator& rhs) {
		return lhs.m_iterator != rhs.m_iterator;
	}
	friend bool operator==(const Dictionary_Iterator& lhs, const Dictionary_Iterator& rhs) {
		return lhs.m_iterator == rhs.m_iterator;
	}
};
