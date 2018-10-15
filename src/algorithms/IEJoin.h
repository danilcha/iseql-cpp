#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include "model/Relation.h"



enum class Operator { Greater, GreaterOrEquals, Less, LessOrEquals };


struct GetIntervalStart
{
	Timestamp operator () (const Interval& interval) const noexcept
	{
		return interval.start;
	}
};


struct GetIntervalEnd
{
	Timestamp operator () (const Interval& interval) const noexcept
	{
		return interval.end;
	}
};




class BitVector
{
public:
	static constexpr size_t END = std::numeric_limits<size_t>::max();

private:
	std::vector<bool> bits;

public:
	BitVector(size_t size)
	:
		bits(size)
	{
	}


	void setBit(size_t i) noexcept
	{
		bits[i] = true;
	}


	size_t findFirstBitInRange(size_t offsetBegin, size_t offsetEnd) noexcept
	{
		auto begin = bits.begin() + static_cast<ptrdiff_t>(offsetBegin);
		auto end   = bits.begin() + static_cast<ptrdiff_t>(offsetEnd);
		auto pos = std::find(begin, end, true);

		if (pos == end)
			return END;
		else
			return static_cast<size_t>(std::distance(bits.begin(), pos));
	}


	size_t size() const noexcept
	{
		return bits.size();
	}
};




class MultilevelBitVector
{
private:
	static constexpr size_t CHUNK_SIZE = 1024;

	BitVector bits;
	BitVector index;
	size_t maxIndexPos = 0;


public:
	MultilevelBitVector(size_t size)
	:
		bits(size),
		index((size + CHUNK_SIZE - 1) / CHUNK_SIZE)
	{
	}


	void setBit(size_t i) noexcept
	{
		bits.setBit(i);

		auto indexPos = i / CHUNK_SIZE;
		index.setBit(indexPos);
		maxIndexPos = std::max(maxIndexPos, indexPos);
	}


	size_t findFirstBitStartingAt(size_t offsetBegin) noexcept
	{
		auto indexBegin = offsetBegin / CHUNK_SIZE;
		if (indexBegin > maxIndexPos)
			return BitVector::END;

		auto indexPos = index.findFirstBitInRange(indexBegin, maxIndexPos + 1);
		if (indexPos == BitVector::END)
			return BitVector::END;

		auto offsetChunkBegin = indexPos * CHUNK_SIZE;

		offsetBegin = std::max(offsetBegin, offsetChunkBegin);
		auto offsetEnd = std::min(bits.size(), offsetChunkBegin + CHUNK_SIZE);
		auto result = bits.findFirstBitInRange(offsetBegin, offsetEnd);
		if (result == BitVector::END && offsetEnd != bits.size())
			return findFirstBitStartingAt(offsetEnd);
		else
			return result;
	}

};








/**
 * The IEJoin algorithm described in the article (Z. Khayyat et al. Fast and scalable
 * inequality joins. VLDB journal, 2017) is incorrect. First of all, there is a typo in
 * line 17: it should be size(L2') or size(L1') or simply n, not size(L2). But that's
 * not the main problem. It contains more principal bugs. It marks marks those tuples suitable
 * for reporting for which the first condition is ‘<=’ when ‘<’ is used at the operator.
 * And it does not support relations with non-distinct attributes. Is magically works
 * for the example data, but fails for other data.
 *
 * This is a fixed version that only supports ‘<’ as the first and ‘>’ as the second operator.
 */

template <typename GetRA1, Operator op1, typename GetSA1,
		  typename GetRA2, Operator op2, typename GetSA2>
class IEJoinOperator2
{
private:
	static_assert(op1 == Operator::Less && op2 == Operator::Greater,
		"This implementation only supports ‘<’ as the first and ‘>’ as the second operator.");

	struct Element
	{
		Timestamp value;
		Tuple tuple;

		Element(Timestamp value, const Tuple& tuple)
		:
			value(value),
			tuple(tuple)
		{
		}
	};

	using Elements = std::vector<Element>;
	using Indices  = std::vector<unsigned>;

	Elements L2;
	Elements L1p;
	Indices P, Pp;
	Indices O1, O2;
	MultilevelBitVector Bp;


public:
	IEJoinOperator2(const Relation& R, const Relation& S)
	:
		Bp(S.size())
	{
		Elements L1;
		Elements L2p;

		relationToElements<GetRA1>(R, L1);
		relationToElements<GetRA2>(R, L2);
		relationToElements<GetSA1>(S, L1p);
		relationToElements<GetSA2>(S, L2p);

		sortAsc(L1);
		sortAsc(L2);
		sortAsc(L1p);
		sortAsc(L2p);

		P  = computePermutations(L2,  L1 );
		Pp = computePermutations(L2p, L1p);

		O1 = computeOffsets(L1, L1p, 1);
		O2 = computeOffsets(L2, L2p);
	}


	template <typename Consumer>
	void join(Consumer&& consumer)
	{
		for (size_t i = 0; i < O1.size(); i++)
		{
			auto off2 = static_cast<size_t>(O2[i]);
			for (size_t j = 0; j < off2; j++)
			{
				auto p = Pp[j];
				Bp.setBit(p);
			}

			auto k = static_cast<size_t>(O1[P[i]]);
			for (;;)
			{
				k = Bp.findFirstBitStartingAt(k);
				if (k == BitVector::END)
					break;

				consumer(L2[i].tuple, L1p[k].tuple);

				k++;
			}
		}

	}

private:
	template <typename GetValue>
	static void relationToElements(const Relation& relation, std::vector<Element>& result)
	{
		GetValue getValue;
		result.reserve(relation.size());

		for (const auto& tuple : relation)
		{
			result.push_back(Element{getValue(tuple), tuple});
		}
	}


	static void sortAsc(Elements& elements)
	{
		std::sort(elements.begin(), elements.end(), [] (const Element& lhs, const Element& rhs) noexcept
		{
			return lhs.value < rhs.value;
		});
	}


	static Indices computePermutations(const Elements& from, const Elements& to)
	{
		auto n = from.size();

		std::unordered_map<int, unsigned> map;
		map.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			map.emplace(to[i].tuple.getId(), (unsigned) i);
		}

		Indices result;
		result.reserve(n);
		for (const Element& item : from)
		{
			result.push_back(map[item.tuple.getId()]);
		}

		return result;
	}

	static Indices computeOffsets(const Elements& from, const Elements& to, Timestamp offset = 0)
	{
		auto f = from.begin();
		auto t = to.begin();

		Indices result;
		result.reserve(from.size());

		while (f != from.end() || t != to.end())
		{
			if (f == from.end() || (t != to.end() && t->value < f->value + offset))
			{
				t++;
			}
			else
			{
				result.push_back(static_cast<unsigned>(std::distance(to.begin(), t)));
				f++;
			}
		}

		return result;
	}

};


using IEJoinStartPrecedingStrict = IEJoinOperator2<
	GetIntervalStart, Operator::Less,    GetIntervalStart,
	GetIntervalEnd,   Operator::Greater, GetIntervalStart>;

using IEJoinReverseDuringStrict = IEJoinOperator2<
	GetIntervalStart, Operator::Less,    GetIntervalStart,
	GetIntervalEnd,   Operator::Greater, GetIntervalEnd>;


template <typename Consumer>
void ieJoinStartPrecedingStrictJoin(const Relation& R, const Relation& S, Consumer&& consumer) noexcept
{
	IEJoinStartPrecedingStrict(R, S).join(consumer);
}


template <typename Consumer>
void ieJoinReverseDuringStrictJoin(const Relation& R, const Relation& S, Consumer&& consumer) noexcept
{
	IEJoinReverseDuringStrict(R, S).join(consumer);
}



