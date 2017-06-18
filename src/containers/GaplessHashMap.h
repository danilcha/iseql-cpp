#pragma once

#include <cstdint>
#include "Util.h"


// todo: bug: we remove the first element in the bucket and the last element in the bucket is moved


template<typename K, typename V>
class GaplessHashMap
{
private:
	typedef int32_t Ref;

	struct Node
	{
		Ref prev, next;
		K key;
	};


	const size_t tableSize;
	const size_t hashMask;
	Ref* const table;
	Ref* const tableBase;

	Node* const nodes;
	V*    const values;
	Node* tail;
	V*    valuesTail;
	Node* bound;
	Node* const nodesBase;
	V*    const valuesBase;

public:
	using value_type = V;
	using const_iterator = const V*;


	GaplessHashMap(size_t capacity) noexcept
	:
		tableSize(next_power_of_two(capacity)),
		hashMask(tableSize - 1),
		table(array_calloc<Ref>(tableSize)),
		tableBase(table + tableSize),

		nodes (array_malloc<Node>(capacity)),
		values(array_malloc<V>(capacity)),
		tail(nodes),
		valuesTail(values),
		bound(nodes + capacity),
		nodesBase(nodes - 1), // for 1-based indexing
		valuesBase(values - 1) // for 1-based indexing
	{
	}


	~GaplessHashMap() noexcept
	{
		std::free(table);
		std::free(nodes);
		std::free(values);
	}


	void insert(K key, const V& value) noexcept
	{
		Ref* slot = table + position(key);
		Ref slotRef = Ref(slot - tableBase);

		Node* node = tail;
		assert(tail < bound);
		tail++;

		Ref nodeRef = Ref(node - nodesBase);

		node->prev = slotRef;
		node->next = *slot;
		node->key  = key;
		if (node->next)
			nodesBase[node->next].prev = nodeRef;
		*slot = nodeRef;

		*valuesTail = value;
		valuesTail++;
	}



	void erase(K key) noexcept
	{
		Ref* slot = table + position(key);
		Ref slotRef = Ref(slot - tableBase);

		Node* node = nodesBase + *slot;


		while (node->key != key) // Will contain the key, may skip checks for null
		{
			slotRef = *slot;
			slot = &node->next;
			node = nodesBase + *slot;
		}

		V* currentValue = valuesBase + *slot;

		*slot = node->next;
		if (node->next)
			nodesBase[node->next].prev = slotRef;

		tail--;
		valuesTail--;
		if (node != tail)
		{
			*currentValue = *valuesTail;
			node->prev  = tail->prev;
			node->next  = tail->next;
			node->key   = tail->key;
			Ref nodeRef = Ref(node - nodesBase);

			if (node->next)
				nodesBase[node->next].prev = nodeRef;

			if (node->prev < 0)
				tableBase[node->prev] = nodeRef;
			else
				nodesBase[node->prev].next = nodeRef;
		}
	}









	inline size_t position(K key) const noexcept
	{
		return static_cast<size_t>(key) & hashMask;
	}


	const_iterator begin() const noexcept
	{
		return values;
	}


	const_iterator end() const noexcept
	{
		return valuesTail;
	}


	size_t size() const noexcept
	{
		return end() - begin();
	}
};

