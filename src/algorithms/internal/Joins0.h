#pragma once

#include <unordered_map>
#include <iostream>
#include "algorithms/internal/Iterators.h"



template <typename IteratorR, typename IteratorS, typename Compare, typename Consumer>
void joinByS(const Relation& R, const Relation& S, IteratorR itR, IteratorS itS, Compare comp, const Consumer& consumer)
{
	std::unordered_map<TID, Tuple> activeR;

	for (;;)
	{
		if (comp(itR.getEndpoint(), itS.getEndpoint()))
		{
			Endpoint endpointR = itR.getEndpoint();
			TID tid = endpointR.getTID();

			if (endpointR.isStart())
				activeR.emplace(tid, R[tid]);
			else
				activeR.erase(tid);

			if (!itR.next())
				break;
		}
		else
		{
			const Tuple& s = S[itS.getEndpoint().getTID()];
			for (const auto& r : activeR)
				consumer(r.second, s);

			if (!itS.next())
				break;
		}
	}
}
