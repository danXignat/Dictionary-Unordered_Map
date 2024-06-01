#pragma once
#include <memory>
#include <list>
#include <tuple>
#include <iostream>
#include <queue>

#include "Unordered_Map_Iterator.h"

template <typename Unordered_Map>
class Unordered_Map_Iterator;

template<typename Key,
	typename T,
	class Hash = std::hash<Key>,
	size_t TableSize = 8>
class Unordered_Map
{
private: // Declaration
	template <typename Unordered_Map>
	friend class Unordered_Map_Iterator;

	using Key_Type = Key;
	using Mapped_Type = T;
	using Pair_Type = std::pair<Key_Type, Mapped_Type>;

	using List = std::list<Pair_Type>;
	struct Table_Tuple {
		typename List::iterator begin;
		typename List::iterator end;
		bool visited = false;
	};

	Table_Tuple* m_hashTable;
	List m_buckets;

	Hash m_Hasher;
	size_t m_size;
	size_t m_bucket_count;
	float m_max_load_factor;

public: // Constructor, Destructor
	Unordered_Map()
		:	m_hashTable(new Table_Tuple[TableSize]),
			m_size(0),
			m_bucket_count(TableSize),
			m_max_load_factor(1.0){}
	~Unordered_Map() {
		delete[] m_hashTable;
	}

public: // Iterator
	using Iterator = Unordered_Map_Iterator<Unordered_Map<Key, T, Hash, TableSize>>;

	Iterator begin() {
		return Iterator(m_buckets.begin());
	}
	Iterator end() {
		return Iterator(m_buckets.end());
	}

public: // Methods
	//Insertion, deletion
	void insert(const Pair_Type& element) {
		size_t position = m_Hasher(element.first) % m_bucket_count;

		if (m_buckets.empty())
		{
			m_buckets.push_front(element);
			m_hashTable[position].begin = m_buckets.begin();
			m_hashTable[position].end = m_buckets.end();

			/*for (const auto& el : m_buckets)
				std::cout << '[' << el.first << "] = " << el.second << " ";
			std::cout << std::endl;*/
		}
		else
		{
			if (m_hashTable[position].visited)
			{
				auto endIterator = m_hashTable[position].end;
				m_buckets.insert(endIterator, element);
			}
			else
			{
				auto tempIterator = m_buckets.begin();
				m_buckets.push_front(element);
				m_hashTable[position].begin = m_buckets.begin();
				m_hashTable[position].end = tempIterator;
			}
		}

		m_hashTable[position].visited = true;
		m_size++;

		if (float(m_size) / m_bucket_count >= m_max_load_factor) {
			this->rehash();
		}
	}
	template<typename... Args>
	void emplace(Args&&... args) {
		Pair_Type element = Pair_Type(std::forward<Args>(args)...);

		this->insert(element);
	}
	void erase(const Key_Type& key) {
		Iterator foundIt = this->find(key);
		if (foundIt != this->end()) {
			typename List::iterator bucketIt = foundIt.bucketIterator();
			m_buckets.erase(bucketIt);
		}

		m_size--;
	}

	//Access 
	Iterator find(const Key_Type& key) {
		size_t position = m_Hasher(key) % m_bucket_count;

		auto it = m_hashTable[position].begin;
		for (; it != m_hashTable[position].end; it++) {
			if ((*it).first == key)
				return Iterator(it);
		}

		return this->end();
	}
	bool contains(const Key_Type& key) {
		return this->find(key) != this->end();
	}
	Mapped_Type& operator[](const Key_Type& key) {
		Iterator foundIt = this->find(key);

		if (foundIt == this->end()) {
			
			this->insert({ key, Mapped_Type()});
			foundIt = this->find(key);
		}
		
		return foundIt->second;
	}
	const Mapped_Type& operator[](const Key_Type& key) const {
		Iterator foundIt = this->find(key);
		
		return foundIt->second;
	}

	//Size, buckets, load_factor
	size_t size() const {
		return m_size;
	}
	size_t bucket_count() const {
		return m_bucket_count;
	}
	float max_load_factor() const {
		return m_max_load_factor;
	}
	void max_load_factor(const float& new_max_lf) {
		m_max_load_factor = new_max_lf;
	}
	float load_factor() const {
		return float(m_size) / m_bucket_count;
	}

	//Memory management rehashing
	void rehash() {
		m_bucket_count *= 8;
		delete[] m_hashTable;
		m_hashTable = new Table_Tuple[m_bucket_count];

		std::queue<Pair_Type> temp;
		for (auto it = m_buckets.begin(); it != m_buckets.end(); it++, m_buckets.pop_front()) {
			temp.push(*it);
		}

		for (; !temp.empty(); temp.pop()) {
			this->insert(temp.front());
		}
	}
};

