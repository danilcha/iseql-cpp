#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include "model/Relation.h"



enum class JoinCondition
{
	GreaterThan, GreaterThanEqual, LessThan, LessThanEqual
};



class IEJoinMasterOperator
{
public:
	bool list1ASC = false;
	bool list1ASCSec = false;
	bool list2ASC = false;
	bool list2ASCSec = false;
	bool equalReverse = false;


	IEJoinMasterOperator(JoinCondition cond0, JoinCondition cond1)
	{
		if (cond0 == JoinCondition::LessThan)
		{
			list1ASC = true;
			list2ASCSec = true;
		}
		else if (cond0 == JoinCondition::LessThanEqual)
		{
			list1ASC = true;
			list2ASCSec = false;
		}
		else if (cond0 == JoinCondition::GreaterThan)
		{
			list1ASC = false;
			list2ASCSec = false;
		}
		else if (cond0 == JoinCondition::GreaterThanEqual)
		{
			list1ASC = false;
			list2ASCSec = true;
		}

		// Reference and secondary pivot sort order
		if (cond1 == JoinCondition::GreaterThan)
		{
			list2ASC = true;
			list1ASCSec = true;
		}
		else if (cond1 == JoinCondition::GreaterThanEqual)
		{
			list2ASC = true;
			list1ASCSec = false;
		}
		else if (cond1 == JoinCondition::LessThan)
		{
			list2ASC = false;
			list1ASCSec = false;
		}
		else if (cond1 == JoinCondition::LessThanEqual)
		{
			list2ASC = false;
			list1ASCSec = true;
		}

		// For equal pivot and reference
		if (list1ASC != list2ASCSec && list2ASC != list1ASCSec)
		{
			equalReverse = true;
		}
	}
};


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


template <typename Get0Pivot, typename Get1Pivot,
		  typename Get0Ref,   typename Get1Ref>
class IEJoinOperator : public IEJoinMasterOperator
{
private:
	struct Element
	{
		Timestamp pivot;
		Timestamp ref;
		const Tuple* tuple;
		size_t rowId;

		Element(Timestamp pivot, Timestamp ref, const Tuple& tuple)
		:
			pivot(pivot),
			ref(ref),
			tuple(&tuple)
		{
		}
	};




public:
	IEJoinOperator(JoinCondition cond0,
				   JoinCondition cond1)
	:
		IEJoinMasterOperator(cond0, cond1)
	{
	}


	template <typename Output>
	void join(const Relation& stream0, const Relation& stream1, const Output& /*output*/)
	{
		auto list0 = streamToElementsAndSort(stream0, Get0Pivot(), Get0Ref(), list1ASC, list1ASCSec);
		auto list1 = streamToElementsAndSort(stream1, Get1Pivot(), Get1Ref(), list2ASC, list2ASCSec);


		auto partCount = list0.size();
		auto partCount2 = list1.size();

		// count starting point for rdd2
		auto cnt2 = partCount + 1;

		// Give unique ID for rdd1 & rdd2
		for (size_t i = 0; i < partCount; i++)
		{
			list0[i].rowId = i;
		}
		for (size_t i = 0; i < partCount2; i++)
		{
			list1[i].rowId = i + cnt2;
		}

	}

private:
	template <typename GetPivot, typename GetRef>
	static std::vector<Element> streamToElementsAndSort(const Relation& stream, GetPivot getPivot, GetRef getRef, bool asc1, bool asc2)
	{
		std::vector<Element> result;
		result.reserve(stream.size());

		for (const auto& tuple : stream)
		{
			result.push_back(Element{getPivot(tuple), getRef(tuple), tuple});
		}

		std::sort(result.begin(), result.end(), [asc1, asc2] (const Element& element1, const Element& element2)
		{
			if (element1.pivot < element2.pivot) return asc1;
			if (element1.pivot > element2.pivot) return !asc1;

			if (element1.ref < element2.ref) return asc1;
			if (element1.ref > element2.ref) return !asc1;

			return true;
		});

		return result;
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


	bool isSet(size_t i) const noexcept
	{
		return bits[i];
	}

	size_t findBitStartingFrom(size_t i) noexcept
	{
		if (i >= bits.size())
			return END;

		auto start = bits.begin() + static_cast<ptrdiff_t>(i);
		auto pos = std::find(start, bits.end(), true);

		if (pos == bits.end())
			return END;
		else
			return static_cast<size_t>(std::distance(bits.begin(), pos));
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

template <typename Get0Pivot, JoinCondition op1, typename Get1Pivot,
		  typename Get0Ref,   JoinCondition op2, typename Get1Ref>
class IEJoinOperator2
{
private:
	static_assert(op1 == JoinCondition::LessThan && op2 == JoinCondition::GreaterThan,
		"This implementation only supports ‘<’ as the first and ‘>’ as the second operator.");

	struct Element
	{
		Timestamp value;
		const Tuple* tuple;

		Element(Timestamp value, const Tuple& tuple)
		:
			value(value),
			tuple(&tuple)
		{
		}

		bool operator < (const Element& other) const noexcept
		{
			return value < other.value;
		}
	};

	using Elements = std::vector<Element>;
	using Indices  = std::vector<unsigned>;

	Elements L2;
	Elements L1p;
	Indices P, Pp;
	Indices O1, O2;
	BitVector Bp;


public:
	IEJoinOperator2(const Relation& r1, const Relation& r2)
	:
		Bp(r2.size())
	{
		Elements L1;
		Elements L2p;

		relationToElements<Get0Pivot>(r1, L1);
		relationToElements<Get1Pivot>(r2, L1p);
		relationToElements<Get0Ref  >(r1, L2);
		relationToElements<Get1Ref  >(r2, L2p);

		sortAsc(L1);
		sortAsc(L1p);
		sortAsc(L2);
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

			auto off1 = static_cast<size_t>(O1[P[i]]);

			for (auto k = off1; k < L1p.size(); k++)
			{
				if (Bp.isSet(k))
					consumer(*L2[i].tuple, *L1p[k].tuple);
			}
//			for (;;)
//			{
//				k = Bp.findBitStartingFrom(k);
//				if (k == BitVector::END)
//					break;
//
//				consumer(*L2[i].tuple, *L2p[k].tuple);
////				std::cout << *L2[i].tuple << ' ' << *L2p[k].tuple << std::endl;
//
//				k++;
//			}
		}

	}

private:
	template <typename GetValue>
	static void relationToElements(const Relation& relation, std::vector<Element>& result)
	{
		result.reserve(relation.size());

		for (const auto& tuple : relation)
		{
			result.push_back(Element{GetValue()(tuple), tuple});
		}
	}


	static void sortAsc(Elements& elements)
	{
		std::sort(elements.begin(), elements.end());
	}


	static Indices computePermutations(const Elements& from, const Elements& to)
	{
		auto n = from.size();

		std::unordered_map<const Tuple*, unsigned> map;
		map.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			map.emplace(to[i].tuple, (unsigned) i);
		}

		Indices result;
		result.reserve(n);
		for (const Element& item : from)
		{
			result.push_back(map.at(item.tuple));
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
	GetIntervalStart, JoinCondition::LessThan,    GetIntervalStart,
	GetIntervalEnd,   JoinCondition::GreaterThan, GetIntervalStart>;

using IEJoinReverseDuringStrict = IEJoinOperator2<
	GetIntervalStart, JoinCondition::LessThan,    GetIntervalStart,
	GetIntervalEnd,   JoinCondition::GreaterThan, GetIntervalEnd>;


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



