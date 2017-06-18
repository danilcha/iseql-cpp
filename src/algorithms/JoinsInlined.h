#pragma once


#include <unordered_map>
#include <iostream>
#include "model/Index.h"
#include "model/Relation.h"


//void afterJoin(const Relation& R, const Relation& S, Timestamp delta, const Consumer& consumer) noexcept
//{
//	beforeJoin(S, R, delta, makeReversingConsumer(consumer));
//}



static Timestamp beforeJoinInlined(const Relation& R, const Relation& S, Timestamp delta)
{
	Timestamp result = 0;

	std::unordered_map<TID, Tuple> activeR;

	const auto& indexR = R.getIndex();
	const auto& indexS = S.getIndex();

	const auto indexREnd = indexR.end();
	const auto indexSEnd = indexS.end();

	auto itR1 = indexR.begin();
	auto itR2 = indexR.begin();
	auto itS  = indexS.begin();

	const auto itR1Shift = Endpoint::calculateShiftArgument(1, Endpoint::Type::END, Endpoint::Type::START);
	const auto itR2Shift = Endpoint::calculateShiftArgument(delta);

	while (itR1->isStart())
		++itR1;
	Endpoint endpointR = itR1->shiftedBy(itR1Shift);
	do
		++itR1;
	while (itR1->isStart());

	while (itR2->isStart())
		++itR2;

	while (itS->isEnd()) // useless, because any index starts with a 'start' endpoint
		++itS;

	for (;;)
	{
		if (endpointR <= *itS)
		{
			TID tid = endpointR.getTID();

			if (endpointR.isStart())
				activeR.emplace(tid, R[tid]);
			else
				activeR.erase(tid);

			auto endpointR1 = itR1->shiftedBy(itR1Shift);
			auto endpointR2 = itR2->shiftedBy(itR2Shift);

			if (itR1 != indexREnd && endpointR1 < endpointR2)
			{
				endpointR = endpointR1;
				do
					++itR1;
				while (itR1 != indexREnd && itR1->isStart());
			}
			else
			{
				endpointR = endpointR2;
				do
					++itR2;
				while (itR2 != indexREnd && itR2->isStart());
			}

			if (itR2 == indexREnd)
				break;
		}
		else
		{
			const Tuple& s = S[itS->getTID()];
			for (const auto& r : activeR)
			{
//				std::cout << "Result " << r.second << "  " << s << std::endl;
				result += r.second.start + s.end;
			}

			do
				++itS;
			while (itS != indexSEnd && itS->isEnd());

			if (itS == indexSEnd)
				break;
		}
	}

	return result;
}
