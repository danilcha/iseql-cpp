#pragma once

#include <iostream>
#include <unordered_map>
#include "model/RelationGenerator.h"
#include "util/Experiments.h"
#include "util/Arguments.h"
#include "util/Aggregates.h"
#include "algorithms/Joins.h"
#include "MainCommon.h"



static void stats(Experiments& experiments, const Relation& relation, const std::string& name)
{
	Aggregate<Timestamp> aggregate;

	for (const auto& tuple : relation)
	{
		aggregate.add(tuple.getLength());

	}

	experiments.addExperimentResult(name + "-avg-len", aggregate.getAvg());
	experiments.addExperimentResult(name + "-max-len", aggregate.max);
}


static void tupleEffectiveEnds(Relation& relation)
{
	GaplessHashMap<TID, Timestamp> active(4096);

	for (const auto& endpoint : relation.getIndex())
	{
		auto tid = endpoint.getTID();
		auto& tuple = relation[tid];

		if (endpoint.isStart())
		{
			active.insert(tid, tuple.end);

//			std::cout << &tuple - &relation[0] << std::endl;


			auto effectiveEnd = std::max_element(active.begin(), active.end());
			tuple.id = *effectiveEnd;
		}
		else
		{
			active.erase(tid);
		}
	}
}


inline void mainLatency(Arguments& arguments)
{
	Relation R, S;

	auto experiments = getExperimentsAndPopulateRelations(arguments, R, S);

	Index indexR{R}, indexS{S};
	R.setIndex(indexR);
	S.setIndex(indexS);

	stats(experiments, R, "r");
	stats(experiments, S, "s");

	Aggregate<Timestamp> latencies1;
	Aggregate<Timestamp> latencies2;

	tupleEffectiveEnds(R);
	tupleEffectiveEnds(S);

	reverseDuringStrictJoin(R, S, [&] (const Tuple& r, const Tuple& s)
	{
		latencies1.add(r.end - s.end);

		auto realEnd = std::max(r.id, s.id);

		latencies2.add(realEnd - s.end);
	});

	experiments.addExperimentResult("latency-1-avg", latencies1.getAvg());
	experiments.addExperimentResult("latency-1-max", latencies1.max);

	experiments.addExperimentResult("latency-2-avg", latencies2.getAvg());
	experiments.addExperimentResult("latency-2-max", latencies2.max);
}



