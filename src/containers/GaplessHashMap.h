#pragma once

#include <cstdint>
#include <algorithm>
#include "Util.h"










template<typename K, typename V>
class GaplessHashMap
{
private:
	class Reference
	{
	private:
		typedef uint32_t value_type;
		static constexpr value_type tableFlag = ~(~value_type(0) >> 1);

	private:
		value_type value;

		Reference(value_type value) noexcept
		:
			value(value)
		{

		}

	public:
		static Reference forTable(size_t position) noexcept
		{
			return static_cast<value_type>(position) | tableFlag;
		}

		static Reference forNodes(size_t position) noexcept
		{
			return static_cast<value_type>(position);
		}

		inline bool isTableReference() const noexcept
		{
			return (value & tableFlag) != 0;
		}

		inline size_t getTablePosition() const noexcept
		{
			return value & ~tableFlag;
		}

		inline size_t getNodesPosition() const noexcept
		{
			return value;
		}

		inline operator bool() const noexcept
		{
			return value != 0;
		}
	};


	template <typename T>
	class BaseOneDynamicArray
	{
		T* array;
		T* arrayEnd;

	public:
		BaseOneDynamicArray(size_t capacity) noexcept
		:
			array(array_malloc<T>(capacity) - 1),
			arrayEnd(begin())
		{
		}

		virtual ~BaseOneDynamicArray() noexcept
		{
			std::free(begin());
		}

		T* insert() noexcept
		{
			auto result = arrayEnd;
			++arrayEnd;
			return result;
		}

		void insert(const T& value) noexcept
		{
			*insert() = value;
		}

		void erase(Reference reference) noexcept
		{
			T* element = &operator[](reference);

			arrayEnd--;

			if (element != arrayEnd)
				*element = *arrayEnd;
		}

		Reference referenceOf(const T* element) const noexcept
		{
			return Reference::forNodes(static_cast<size_t>(element - array));
		}

		T& operator[](Reference reference) noexcept
		{
			return array[reference.getNodesPosition()];
		}

		T* begin() const noexcept
		{
			return array + 1;
		}

		T* end() const noexcept
		{
			return arrayEnd;
		}

		size_t size() const noexcept
		{
			return static_cast<size_t>(end() - begin());
		}

		void resize(size_t capacity)
		{
			size_t saveSize = size();
			array = array_realloc(begin(), capacity) - 1;
			arrayEnd = begin() + saveSize;
		}
	};


	class HashTable
	{
		Reference* table;
		size_t mask;

	public:
		HashTable(size_t size) noexcept
		:
			table(array_calloc<Reference>(size)),
			mask(size - 1)
		{
		}


		friend void swap(HashTable& table1, HashTable& table2)
		{
			std::swap(table1.table, table2.table);
			std::swap(table1.mask,  table2.mask);
		}

		~HashTable() noexcept
		{
			std::free(table);
		}


		inline size_t size() const noexcept
		{
			return mask + 1;
		}


		Reference referenceOf(const Reference* bucket) const noexcept
		{
			return Reference::forTable(static_cast<size_t>(bucket - table));
		}


		Reference* bucketFor(K key) noexcept
		{
			size_t position = static_cast<size_t>(key) & mask;
			return table + position;
		}

		Reference& operator[](Reference reference) noexcept
		{
			return table[reference.getTablePosition()];
		}

	};


	struct Node
	{
		Reference prev, next;
		K key;
	};


private:
	size_t capacity;
	HashTable table;
	BaseOneDynamicArray<Node> nodes;
	BaseOneDynamicArray<V> values;


public:
	using value_type = V;
	using const_iterator = const V*;

public:
	GaplessHashMap(size_t capacity) noexcept
	:
		capacity(capacity),
		table(next_power_of_two(capacity)),
		nodes (capacity),
		values(capacity)
	{
	}




	void insert(K key, const V& value) noexcept
	{
		if (values.size() == capacity)
			grow();

		values.insert(value);
		Node* node = nodes.insert();

		node->key = key;
		insertNodeIntoBucket(table, node);
	}



	void erase(K key) noexcept
	{
		Reference reference = *table.bucketFor(key);
		Node* node = &nodes[reference];

		// Find the node

		while (node->key != key)
		{
			reference = node->next;
			node = &nodes[reference];
		}

		// Remove the node from the bucket

		Reference prev = node->prev;
		Reference next = node->next;

		if (prev.isTableReference())
			table[prev] = next;
		else
			nodes[prev].next = next;

		if (next)
			nodes[next].prev = prev;

		// Remove the node and the value from storage arrays

		values.erase(reference);
		nodes.erase(reference);

		if (node != nodes.end()) // node now points to a different, moved node
		{
			prev = node->prev;
			next = node->next;

			if (prev.isTableReference())
				table[prev] = reference;
			else
				nodes[prev].next = reference;

			if (next)
				nodes[next].prev = reference;
		}
	}






	size_t size() const noexcept
	{
		return values.size();
	}




	const_iterator begin() const noexcept
	{
		return values.begin();
	}


	const_iterator end() const noexcept
	{
		return values.end();
	}

private:
	void grow() noexcept
	{
		constexpr size_t factor = 4;

		capacity *= factor;
		nodes.resize(capacity);
		values.resize(capacity);

		HashTable table2(table.size() * factor);

		for (Node& node : nodes)
		{
			insertNodeIntoBucket(table2, &node);

		}


		swap(table, table2);
	}



	void insertNodeIntoBucket(HashTable& table, Node* node) noexcept
	{
		Reference* bucket = table.bucketFor(node->key);

		node->prev = table.referenceOf(bucket);
		node->next = *bucket;
		if (node->next)
			nodes[node->next].prev = nodes.referenceOf(node);

		*bucket = nodes.referenceOf(node);

	}



};

