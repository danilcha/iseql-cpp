#pragma once

#include <unordered_map>
#include <iostream>
#include "algorithms/Iterators.h"




template <typename IteratorR, typename IteratorS, typename Compare, typename Consumer>
void joinByS(const Relation& R, const Relation& S, IteratorR itR, IteratorS itS, Compare comp, const Consumer& consumer)
{
	std::unordered_map<TID, Tuple> activeR;

	while (true)
	{
		if (comp(*itS, *itR))
		{
			const Tuple& s = S[(*itS).getTID()];
			for (const auto& r : activeR)
				consumer(r.second, s);

			if (!++itS)
				break;
		}
		else
		{
			Endpoint endpointR = *itR;

			if (endpointR.isStart())
				activeR.emplace(endpointR.getTID(), R[endpointR.getTID()]);
			else
				activeR.erase(endpointR.getTID());

			if (!++itR)
				break;
		}
	}
}


