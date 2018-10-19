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


static void tupleEffectiveEnds(const Relation& relation, std::unordered_map<int, Timestamp>& result)
{
	GaplessHashMap<TID, Timestamp> active(4096);

	for (const auto& endpoint : relation.getIndex())
	{
		const auto tid = endpoint.getTID();
		const auto& tuple = relation[tid];

		if (endpoint.isStart())
		{
			active.insert(tid, tuple.end);

//			std::cout << &tuple - &relation[0] << std::endl;


			auto effectiveEnd = std::max_element(active.begin(), active.end());
			result.emplace(tuple.id, *effectiveEnd);
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

	{
		int i = 1;
		for (auto& tuple : R)
			tuple.id = i++;
		for (auto& tuple : S)
			tuple.id = i++;
	}

	Index indexR{R}, indexS{S};
	R.setIndex(indexR);
	S.setIndex(indexS);

	stats(experiments, R, "r");
	stats(experiments, S, "s");

	Aggregate<Timestamp> latencies1;
	Aggregate<Timestamp> latencies2;

	std::unordered_map<int, Timestamp> effectiveEnds{4096};

	tupleEffectiveEnds(R, effectiveEnds);
	tupleEffectiveEnds(S, effectiveEnds);

	reverseDuringStrictJoin(R, S, [&] (const Tuple& r, const Tuple& s)
	{
		latencies1.add(r.end - s.end);

		auto realEnd = std::max(
			effectiveEnds.at(r.id),
			effectiveEnds.at(s.id)
		);

		latencies2.add(realEnd - s.end);
	});

	experiments.addExperimentResult("latency-1-avg", latencies1.getAvg());
	experiments.addExperimentResult("latency-1-max", latencies1.max);

	experiments.addExperimentResult("latency-2-avg", latencies2.getAvg());
	experiments.addExperimentResult("latency-2-max", latencies2.max);
}



