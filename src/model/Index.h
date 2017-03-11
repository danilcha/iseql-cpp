#pragma once

#include <vector>
#include <algorithm>
#include "model/Endpoint.h"
#include "model/Relation.h"



class Index : public std::vector<Endpoint>
{
private:
	size_t maxOverlappingTupleCount;


public:
	Index()
	{

	}

	using std::vector<Endpoint>::const_iterator; // Until CLion fixes bug

	Index(const Relation& r)
	{
		buildFor(r);
	}


	void buildFor(const Relation& r)
	{
		reserve(r.size() * 2);

		for (size_t i = 0; i < r.size(); i++)
		{
			emplace_back(r[i].start, Endpoint::Type::START, i);
			emplace_back(r[i].end,   Endpoint::Type::END,   i);
		}

		std::sort(begin(), end());

		calcMaxOverlappingTupleCount();
	}


	size_t getMaxOverlappingTupleCount() const
	{
		return maxOverlappingTupleCount;
	}



private:
	void calcMaxOverlappingTupleCount() noexcept
	{
		size_t    overlappingTupleCount = 0;
		size_t maxOverlappingTupleCount = 0;

		for (const auto& endpoint : *this)
		{
			if (endpoint.isStart())
			{
				overlappingTupleCount++;
				maxOverlappingTupleCount = std::max(maxOverlappingTupleCount, overlappingTupleCount);
			}
			else
				overlappingTupleCount--;
		}

		this->maxOverlappingTupleCount = maxOverlappingTupleCount;
	}
};
