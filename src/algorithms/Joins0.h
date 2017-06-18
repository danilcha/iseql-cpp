#pragma once

#include <iostream>
#include "Iterators.h"
#include "containers/GaplessHashMap.h"



template <typename IteratorR, typename IteratorS, typename Compare, typename Consumer>
void joinByS(const Relation& R, const Relation& S, IteratorR itR, IteratorS itS, Compare comp, const Consumer& consumer)
{
	GaplessHashMap<TID, Tuple> activeR(1024);

	for (;;)
	{
		if (comp(itR.getEndpoint(), itS.getEndpoint()))
		{
			const Endpoint& endpointR = itR.getEndpoint();
			TID tid = endpointR.getTID();

			if (endpointR.isStart())
				activeR.insert(tid, R[tid]);
			else
				activeR.erase(tid);

			itR.moveToNextEndpoint();
			if (itR.isFinished())
				break;
		}
		else
		{
			const Tuple& s = S[itS.getEndpoint().getTID()];
			for (const auto& r : activeR)
				consumer(r, s);

			itS.moveToNextEndpoint();
			if (itS.isFinished())
				break;
		}
	}
}
