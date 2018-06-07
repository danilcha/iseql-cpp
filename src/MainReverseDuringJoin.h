#pragma once

#include <iostream>

#include "util/Arguments.h"
#include "util/Timer.h"
#include "MainCommon.h"
#include "algorithms/Joins.h"
#include "algorithms/LeungMuntzJoins.h"



inline void mainReverseDuringJoin(Arguments& arguments)
{
	Relation R, S;

	if (!arguments.empty())
	{
		Timer timer;
		std::cout << "Relation R: ";
		R = getRelation(arguments, 564564);
		std::cout << "Relation G: ";
		S = getRelation(arguments, 345);
		std::cout << "Loading done in ";
		timer.stopAndPrint();
		std::cout << " s" << std::endl;
	}
	else
	{
//		R = RelationGenerator::generateUniform((unsigned) 1e5, 1, (unsigned) 6e2, 1, (unsigned) 1e9, 1232398);
//		S = RelationGenerator::generateUniform((unsigned) 1e4, 1, (unsigned) 1e2, 1, (unsigned) 1e9, 345);
//		R = RelationGenerator::generateExponential(100'000, 1e-4, 1, 1'000'000, 1232398);
//		S = RelationGenerator::generateExponential(100'000, 1e-4, 1, 1'000'000, 345);
		R = RelationGenerator::generateUniform(1200000, 1, 3, 10, 3000, 5904595);
		S = RelationGenerator::generateUniform( 30000, 1, 3, 10, 3000,   58534);
	}

//	std::unordered_set<Timestamp> ts;
//	for (const auto& r : R)
//	{
//		ts.insert(r.start);
//		ts.insert(r.end);
//	}
//	std::cout << "Distinct: " << ts.size() << std::endl;


	auto disabledExperiments = "-lm0";//arguments.getCurrentArgAndSkipIt("comma-separated list of disabled tests");
	Experiments experiments(disabledExperiments);

	experiments.prepare("sort", "", [&]
	{
	    R.sort();
	    S.sort();
	});

	Index indexR{}, indexS{};

	experiments.prepare("index", "sort", [&]
	{
		indexR.buildFor(R);
		indexS.buildFor(S);
	});

	R.setIndex(indexR);
	S.setIndex(indexS);

//	for (const auto& item : R)
//		std::cout << "r: " << item << std::endl;
//	for (const auto& item : S)
//		std::cout << "s: " << item << std::endl;

	experiments.experiment("danila", "index", [&]
	{
		Timestamp accum = 0;
		reverseDuringStrictJoin(R, S, Workload{accum});
		return accum;
	});


	experiments.experiment("lm0", "sort", [&]
	{
		Timestamp accum = 0;
		leungMuntzReverseDuringStrictJoinOld(R, S, Workload{accum});
		return accum;
	});

	experiments.experiment("lm", "sort", [&]
	{
		Timestamp accum = 0;
		leungMuntzReverseDuringStrictJoin(R, S, Workload{accum});
		return accum;
	});

	experiments.experiment("lm2", "sort", [&]
	{
		Timestamp accum = 0;
		leungMuntzReverseDuringStrictJoin2(R, S, Workload{accum});
		return accum;
	});

	#ifdef COUNTERS
	experiments.addExperimentResult("lm-sel",   (double) lmCounterAfterSelection / lmCounterBeforeSelection);
//	experiments.addExperimentResult("lm-x-avg", (double) lmCounterActiveXCount / lmCounterActiveCountTimes);
//	experiments.addExperimentResult("lm-y-avg", (double) lmCounterActiveYCount / lmCounterActiveCountTimes);
	experiments.addExperimentResult("lm-avg", (double) (lmCounterActiveXCount + lmCounterActiveYCount) / lmCounterActiveCountTimes);
//	experiments.addExperimentResult("lm-x-max", lmCounterActiveXMax);
//	experiments.addExperimentResult("lm-y-max", lmCounterActiveYMax);
	experiments.addExperimentResult("lm-max", lmCounterActiveMax);
	experiments.addExperimentResult("my-sel",   (double) myCounterAfterSelection / myCounterBeforeSelection);
	experiments.addExperimentResult("my-avg", (double) myCounterActiveCount / myCounterActiveCountTimes);
	experiments.addExperimentResult("my-max", myCounterActiveMax);
	#endif
}
