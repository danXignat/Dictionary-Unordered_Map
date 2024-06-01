#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <optional>

#include "Dictionary_Iterator.h"

template<typename Key,
	typename T,
	class Hash = std::hash<Key>,
	size_t TableSize = 8>
class Dictionary
{
private:
	// Declarations //
	
	// Iterator class
	template <typename Dictionary>
	friend class Dictionary_Iterator;

	//Using for inserted values
	using Key_Type = Key;
	using Mapped_Type = T;
	using Pair_Type = std::pair<Key_Type, Mapped_Type>;

	//Hash Table
	enum SlotState {
		OCCUPIED, WAS_OCCUPIED, NEVER_OCCUPIED
	};
	struct Table_Slot {
		size_t index = size_t();
		SlotState state = NEVER_OCCUPIED;
	};
	Table_Slot* m_hash_table;

	//Compact vector to store elements
	struct Tuple_Type {
		size_t hash_value;
		size_t table_index_value;
		Pair_Type pair;

		Tuple_Type()
			:	hash_value(size_t()),
				table_index_value(size_t()),
				pair(Pair_Type()) {}
		Tuple_Type(size_t hash_value, size_t table_index_value, Pair_Type pair)
			:	hash_value(hash_value),
				table_index_value(table_index_value),
				pair(pair) {}
	};
	using Vector = std::vector<Tuple_Type>;
	Vector m_buckets;

	//Hash policy
	Hash m_Hasher;
	size_t m_size;
	size_t m_bucket_count;
	size_t m_contamination_count;
	float m_max_contamination_factor;
	float m_max_load_factor;

public: 
	// CONSTRUCTOR, DESTRUCTOR //
	Dictionary()
		:	m_hash_table(new Table_Slot[TableSize]),
			m_size(0),
			m_bucket_count(TableSize),
			m_max_load_factor(0.7f),
			m_contamination_count(0),
			m_max_contamination_factor(0.1f)
	{
		size_t capacity = size_t(m_max_load_factor * float(m_bucket_count));
		m_buckets.reserve(capacity);
	}
	~Dictionary() {
		delete[] m_hash_table;
	}

public: 
	// ITERATOR //
	using Iterator = Dictionary_Iterator<Dictionary<Key, T, Hash, TableSize>>;

	Iterator begin() {
		return Iterator(m_buckets.begin());
	}
	Iterator end() {
		return Iterator(m_buckets.end());
	}

private:
	// PRIVATE METHODS //
	bool isValidPos(const size_t& index) const {
		return (m_hash_table[index].state != NEVER_OCCUPIED);
	}
	bool isContaminated() const {
		return (m_contamination_count > m_max_contamination_factor * m_bucket_count);
	}
	size_t getTargetHash(const size_t& index) const {
		return m_buckets[m_hash_table[index].index].hash_value;
	}
	void dispersionFunction(size_t& index, size_t& hash_value) {
		index = (5 * index + hash_value + 1) % m_bucket_count;
		hash_value >>= 5;
	}
	void updateTableIndexTo (const size_t& index) { // the value indicated by vector element to hTable
		m_hash_table[m_buckets[index].table_index_value].index = index;
	}
	void updateStateHashTable(SlotState state, const size_t& index) {
		m_hash_table[m_buckets[index].table_index_value].state = state;
	}
	Mapped_Type& getMappedValue(const size_t& index) {
		return m_buckets[m_hash_table[index].index].pair.second;
	}


public: 
	// METHODS //
	
	//Insertion, deletion
	void insert(const Pair_Type& pair) {
		size_t hash_value = m_Hasher(pair.first);
		size_t index = hash_value % m_bucket_count;

		while (m_hash_table[index].state == OCCUPIED) {
			dispersionFunction(index, hash_value);
		}

		if (m_hash_table[index].state == WAS_OCCUPIED) {
			m_contamination_count--;
		}

		m_hash_table[index].state = OCCUPIED;
		m_hash_table[index].index = m_buckets.size();
		m_buckets.emplace_back( hash_value, index, pair );
		m_size++;

		if (float(m_size) / m_bucket_count >= m_max_load_factor) {
			this->reserve();
			this->rehash();
		}
	}

	void erase(const Key_Type& key) {
		size_t hash_value = m_Hasher(key);
		size_t index = hash_value % m_bucket_count;
		size_t target_hash = m_buckets[m_hash_table[index].index].hash_value;
		size_t index_to_delete;

		while (hash_value != target_hash) {
			index = (5 * index + hash_value + 1) % m_bucket_count;
			dispersionFunction(index, hash_value);
		}

		index_to_delete = m_hash_table[index].index;
		m_buckets[index_to_delete] = std::move(m_buckets.back()); // moving data from back to the deleted data index

		updateTableIndexTo(index_to_delete);
		m_hash_table[index].state = WAS_OCCUPIED;
		m_hash_table[index].index = size_t();

		m_buckets.pop_back();
		m_contamination_count++;
		m_size--;

		if (isContaminated()) {
			this->rehash();
		}
	}
	template<typename... Args>
	void emplace(Args&&... args) {
		Pair_Type element = Pair_Type(std::forward<Args>(args)...);

		this->insert(element);
	}

	//Accessors
	const Mapped_Type get(const Key_Type& key)  {
		size_t hash_value = m_Hasher(key);
		size_t index = hash_value % m_bucket_count;

		bool found = false;
		for (size_t t = 0; t < m_bucket_count && !found && isValidPos(index); t++) {
			size_t target_hash = getTargetHash(index);
			if (hash_value == target_hash)
				found = true;
			else
				dispersionFunction(index, hash_value);
		}

		if (found)
			return getMappedValue(index);
		return Mapped_Type();
	}

	Mapped_Type& operator[](const Key_Type& key) {
		size_t hash_value = m_Hasher(key);
		size_t index = hash_value % m_bucket_count;

		bool found = false;
		for (size_t t = 0; t < m_bucket_count && !found && isValidPos(index); t++) {
			size_t target_hash = getTargetHash(index);
			if (hash_value == target_hash)
				found = true;
			else
				dispersionFunction(index, hash_value);
		}

		if (found)
			return getMappedValue(index);

		this->insert({ key, Mapped_Type() });
		return m_buckets.back().pair.second;
	}

	// HASH POLICY //
		
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
	void max_contamination_factor(const float& new_max) {
		m_max_contamination_factor = new_max;
	}
	float contamination_factor() const {
		return float(m_bucket_count) * 100 / m_contamination_count;
	}
	//Rehash
	void reserve() {
		Table_Slot* temp =  new Table_Slot[m_bucket_count * 8];
		std::move(m_hash_table, m_hash_table + m_bucket_count, temp);

		m_bucket_count *= 8;
		delete[] m_hash_table;
		m_hash_table = temp;
	}

	void rehash( ) {
		for (size_t i = 0; i < m_bucket_count; i++)
			m_hash_table[i].state = NEVER_OCCUPIED;

		for (size_t i = 0; i < m_buckets.size(); i++) {
			size_t hash_value = m_buckets[i].hash_value;
			size_t index = hash_value % m_bucket_count;

			while (m_hash_table[index].state == OCCUPIED) {
				dispersionFunction(index, hash_value);
			}

			m_hash_table[index].state = OCCUPIED;
			m_hash_table[index].index = i;
		}
	}

	void print() {
		for (size_t i = 0; i < m_bucket_count; i++)
			if (m_hash_table[i].state == OCCUPIED) {
				std::cout << m_hash_table[i].index << ' ';
			}
			else if (m_hash_table[i].state == WAS_OCCUPIED)
			{
				std::cout << "WAS ";
			}
			else {
				std::cout << "NULL ";
			}
		std::cout << std::endl;
		for (const auto& el : m_buckets)
			std::cout << '[' << el.pair.first << "] = " << el.pair.second << " ";
		std::cout << std::endl;
	}
};

