#pragma once


#include <iostream>

#include "util/Arguments.h"
#include "model/RelationGenerator.h"
#include "util/Experiments.h"

inline Relation getRelation(Arguments& arguments, RelationGenerator::SeedType seed)
{
	std::string type = arguments.getCurrentArgAndSkipIt("relation source type (--file, --uniform, --exp)");

	Relation result;

	if (arguments.isSwitchPresent("--reset-seed"))
		seed = 23423;

	if (type == "--file")
	{
		auto filename = arguments.getCurrentArgAndSkipIt("file name");
		std::cout << "loading from file " << filename << std::flush;
		result = RelationGenerator::loadFromClosedIntervalFile(filename);
	}
	else
	if (type == "--basf")
	{
		auto filename = arguments.getCurrentArgAndSkipIt("file name");
		std::cout << "loading from BASF file " << filename << std::flush;
		result = RelationGenerator::loadFromBASFFile(filename);
	}
	else
	if (type == "--dasa")
	{
		auto filename = arguments.getCurrentArgAndSkipIt("file name");
		std::cout << "loading from DASA file " << filename << std::flush;
		result = RelationGenerator::loadFromDASATemperatureFile(filename);
	}
	else
	if (type == "--uni")
	{
		auto count = arguments.getCurrentArgAsDoubleAndSkipIt("tuple count");
		auto max   = arguments.getCurrentArgAsDoubleAndSkipIt("max tuple length");
		std::cout << "generating " << count << " uniform tuples with max length " << max  << std::flush;
		result = RelationGenerator::generateUniform(size_t(count), 1, Timestamp(max), 1, Timestamp(1e6), seed);

	}
	else
	if (type == "--exp")
	{
		auto count = arguments.getCurrentArgAsDoubleAndSkipIt("tuple count");
		auto max   = arguments.getCurrentArgAsDoubleAndSkipIt("--uni max tuple length analog");
		auto avg   = max / 2;
		std::cout << "generating " << count << " exp tuples with avg length " << avg << std::flush;
		result = RelationGenerator::generateExponential(size_t(count), 1/avg, 1, Timestamp(1e6), seed);
	}
	else
		arguments.error("Unknown relation source ", type);

	std::cout << ", n = " << result.size() << std::flush;

	std::cout << std::endl;
	return result;
}



inline Experiments getExperimentsAndPopulateRelations(Arguments& arguments, Relation& R, Relation& S)
{
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


	auto disabledExperiments = arguments.getCurrentArgAndSkipIt("comma-separated list of disabled tests");
	return Experiments(disabledExperiments);
}



struct Workload
{
	Timestamp& accum;
	#ifdef COUNTERS
	mutable unsigned long long count = 0;
	#endif

	void operator()(const Tuple& r, const Tuple& s) const noexcept
	{
		#ifdef COUNTERS
		count++;
		#endif

		accum += r.start + s.end;
	}
};
