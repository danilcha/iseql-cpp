#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <tuple>
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




class IndexedBitVector
{
private:
	static constexpr size_t CHUNK_SIZE = 1024;

	BitVector bits;
	BitVector index;
	size_t maxIndexPos = 0;


public:
	IndexedBitVector(size_t size)
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
		bool isOuter;
		Tuple tuple;

		Element(Timestamp value, bool isOuter, const Tuple& tuple)
		:
			value(value),
			isOuter(isOuter),
			tuple(tuple)
		{
		}
	};

	using Elements = std::vector<Element>;
	using Indices  = std::vector<unsigned>;

	Elements L1;
	Elements L2;
	Indices P;
	IndexedBitVector B;


public:
	IEJoinOperator2(const Relation& R, const Relation& S)
	:
		B(R.size() + S.size())
	{
		relationsToElements<GetRA1, GetSA1>(R, S, L1);
		relationsToElements<GetRA2, GetSA2>(R, S, L2);

		std::sort(L1.begin(), L1.end(), [] (const Element& lhs, const Element& rhs) noexcept
		{
			return std::make_tuple(lhs.value,  lhs.isOuter) < std::make_tuple(rhs.value,  rhs.isOuter);
		});

		std::sort(L2.begin(), L2.end(), [] (const Element& lhs, const Element& rhs) noexcept
		{
			return std::make_tuple(lhs.value, !lhs.isOuter) < std::make_tuple(rhs.value, !rhs.isOuter);
		});

		computePermutations(L2, L1, P);
	}


	template <typename Consumer>
	void join(Consumer&& consumer)
	{
		for (size_t i2 = 0; i2 < L2.size(); i2++)
		{
			if (!L2[i2].isOuter)
				continue;

			for (size_t j2 = 0; j2 < i2; j2++)
			{
				if (L2[j2].isOuter)
					continue;

				B.setBit(P[j2]);
			}

			auto i1 = static_cast<size_t>(P[i2]);
			for (;;)
			{
				i1 = B.findFirstBitStartingAt(i1 + 1);
				if (i1 == BitVector::END)
					break;

				consumer(L2[i2].tuple, L1[i1].tuple);
			}
		}

	}

private:
	template <typename GetRValue, typename GetSValue>
	static void relationsToElements(const Relation& R, const Relation& S, std::vector<Element>& result)
	{
		GetRValue getRValue;
		GetSValue getSValue;

		result.reserve(R.size() + S.size());

		int i = 1;

		for (const auto& r : R)
		{
			Element element = {getRValue(r), true,  r};
			element.tuple.id = i++;
			result.push_back(element);
		}

		for (const auto& s : S)
		{
			Element element = {getSValue(s), false, s};
			element.tuple.id = i++;
			result.push_back(element);
		}
	}




	static void computePermutations(const Elements& from, const Elements& to, Indices& result)
	{
		auto n = from.size();

		std::unordered_map<int, unsigned> map;
		map.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			map.emplace(to[i].tuple.id, (unsigned) i);
		}

		result.reserve(n);
		for (const Element& item : from)
		{
			result.push_back(map[item.tuple.id]);
		}
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



