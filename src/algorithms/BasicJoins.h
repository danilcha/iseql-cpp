#pragma once

#include <unordered_map>
#include <iostream>
#include "model/Index.h"
#include "model/Relation.h"


using AlgorithmActiveTupleMap = std::unordered_map<TID, Tuple>;


template <typename Consumer>
void startPrecedingJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	const Index& indexR = R.getIndex();
	const Index& indexS = S.getIndex();

	auto endpointR = indexR.begin();
	auto endpointS = indexS.begin();

	AlgorithmActiveTupleMap activeR;

	while (true)
	{
		if (*endpointS < *endpointR) // if equals, do R first
		{
			if (endpointS->isStart())
			{
				const Tuple& s = S[endpointS->getTID()];
				for (const auto& r : activeR)
					consumer(r.second, s);
			}

			endpointS++;
			if (endpointS == indexS.end())
				break;
		}
		else
		{
			if (endpointR->isStart())
				activeR.emplace(endpointR->getTID(), R[endpointR->getTID()]);
			else
				activeR.erase(endpointR->getTID());

			endpointR++;
			if (endpointR == indexR.end())
				break;
		}
	}
}



template <typename Consumer>
void endFollowingJoin(const Relation& R, const Relation& S, const Consumer& consumer)
{
	const Index& indexR = R.getIndex();
	const Index& indexS = S.getIndex();

	auto endpointR = indexR.begin();
	auto endpointS = indexS.begin();

	AlgorithmActiveTupleMap activeR;

	while (true)
	{
		if (*endpointS <= *endpointR) // if equals, do S first
		{
			if (endpointS->isEnd())
			{
				const Tuple& s = S[endpointS->getTID()];
				for (const auto& r : activeR)
					consumer(r.second, s);
			}

			endpointS++;
			if (endpointS == indexS.end())
				break;
		}
		else
		{
			if (endpointR->isStart())
				activeR.emplace(endpointR->getTID(), R[endpointR->getTID()]);
			else
				activeR.erase(endpointR->getTID());

			endpointR++;
			if (endpointR == indexR.end())
				break;
		}
	}
}

