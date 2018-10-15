#pragma once

#include <iostream>
#include <memory>

#include "util/Arguments.h"
#include "util/Experiments.h"
#include "util/Timer.h"
#include "model/RelationGenerator.h"
#include "MainCommon.h"
#include "algorithms/IEJoin.h"
#include "algorithms/Joins.h"
#include "algorithms/LMJoins.h"



typedef void join_algorithm_t(const Relation&, const Relation&, const Workload&);

inline void compareDanilaAndLM(const Relation& R, const Relation& S, Experiments& experiments,
                               join_algorithm_t* danila, join_algorithm_t* lm)
{
	experiments.experiment("danila", "index", [&]
	{
		Timestamp accum = 0;
		danila(R, S, Workload{accum});
		return accum;
	});

	experiments.experiment("lm", "sort", [&]
	{
		Timestamp accum = 0;

		Workload workload{accum};
		lm(R, S, workload);

		#ifdef COUNTERS
		experiments.addExperimentResult("z", static_cast<double>(workload.count));
		#endif

		return accum;
	});
}


template <typename IEJoin>
void testIEJoin(const Relation& R, const Relation& S, Experiments& experiments)
{
	std::unique_ptr<IEJoin> ieJoin;

	experiments.prepare("ie-index", "", [&]
	{
		ieJoin = std::make_unique<IEJoin>(R, S);
	});

	experiments.experiment("ie", "ie-index", [&]
	{
		Timestamp accum = 0;
		ieJoin->join(Workload{accum});
		return accum;
	});
}



inline void mainJoins(const std::string& command, Arguments& arguments)
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
//		R = RelationGenerator::generateUniform(12, 1, 3, 10, 30, 5904595);
//		S = RelationGenerator::generateUniform( 3, 1, 3, 10, 30,   58534);
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



	if (command == "reverse-during")
	{
		compareDanilaAndLM(R, S, experiments, &reverseDuringStrictJoin,  &leungMuntzReverseDuringStrictJoin);
		testIEJoin<IEJoinReverseDuringStrict>(R, S, experiments);
	}
	else
	if (command == "start-preceding")
	{
		compareDanilaAndLM(R, S, experiments, &startPrecedingStrictJoin, &leungMuntzStartPrecedingStrictJoin);
		testIEJoin<IEJoinStartPrecedingStrict>(R, S, experiments);
	}
	else
	if (command == "left-overlap")
	{
		compareDanilaAndLM(R, S, experiments, &leftOverlapStrictJoin,    &leungMuntzLeftOverlapStrictJoin);
	}
	else
		arguments.error("Invalid command ", command);





	#ifdef COUNTERS
	experiments.addExperimentResult("lm-sel",   (double) lmCounterAfterSelection / (double) lmCounterBeforeSelection);
//	experiments.addExperimentResult("lm-x-avg", (double) lmCounterActiveCountX / lmCounterActiveCountTimes);
//	experiments.addExperimentResult("lm-y-avg", (double) lmCounterActiveCountY / lmCounterActiveCountTimes);
	experiments.addExperimentResult("lm-avg", (double) (lmCounterActiveCountX + lmCounterActiveCountY) / (double) lmCounterActiveCountTimes);
//	experiments.addExperimentResult("lm-x-max", lmCounterActiveMaxX);
//	experiments.addExperimentResult("lm-y-max", lmCounterActiveMaxY);
	experiments.addExperimentResult("lm-max", (double) lmCounterActiveMax);
	experiments.addExperimentResult("my-sel", myCounterBeforeSelection ? (double) myCounterAfterSelection / (double) myCounterBeforeSelection : 1);
	experiments.addExperimentResult("my-avg", (double) myCounterActiveCount / (double) myCounterActiveCountTimes);
	experiments.addExperimentResult("my-max", (double) myCounterActiveMax);
	experiments.addExperimentResult("lm-comp", (double) (lmComparisonCount + lmCounterBeforeSelection * 2));
	experiments.addExperimentResult("my-comp", (double) myCounterBeforeSelection);
	#endif
}
