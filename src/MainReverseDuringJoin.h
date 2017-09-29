#pragma once

#include <iostream>

#include "util/Arguments.h"
#include "util/Timer.h"
#include "MainCommon.h"
#include "algorithms/Joins.h"
#include "algorithms/LeungMuntzJoins.h"



struct Workload
{
	Timestamp& accum;

	void operator()(const Tuple& r, const Tuple& s) const noexcept
	{
		accum += r.start + s.end;
	}
};



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
		R = RelationGenerator::generateExponential(3, 1e-4, 1, 1'000'000, 1232398);
		S = RelationGenerator::generateExponential(3, 1e-4, 1, 1'000'000, 345);
	}


	auto disabledExperiments = "";//arguments.getCurrentArgAndSkipIt("comma-separated list of disabled tests");
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

	for (const auto& item : R)
		std::cout << "r: " << item << std::endl;
	for (const auto& item : S)
		std::cout << "s: " << item << std::endl;

	experiments.experiment("danila", "index", [&]
	{
		Timestamp accum = 0;
		reverseDuringStrictJoin(R, S, Workload{accum});
		return accum;
	});


	experiments.experiment("lm0", "index", [&]
	{
		Timestamp accum = 0;
		leungMuntzReverseDuringStrictJoinOld(R, S, Workload{accum});
		return accum;
	});

	experiments.experiment("lm", "index", [&]
	{
		Timestamp accum = 0;
		leungMuntzReverseDuringStrictJoin(R, S, Workload{accum});
		return accum;
	});
}
