#pragma once


#include <iostream>

#include "util/Arguments.h"
#include "model/Relation.h"

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



struct Workload
{
	Timestamp& accum;

	void operator()(const Tuple& r, const Tuple& s) const noexcept
	{
		accum += r.start + s.end;
	}
};
