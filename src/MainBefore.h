#pragma once

#include <iostream>
#include <model/RelationGenerator.h>
#include "util/Experiments.h"
#include "algorithms/JoinsInlined.h"
#include "algorithms/Joins.h"



template <typename Code>
static void timer(Code code)
{
	Timer timer;

	auto result = code();

	timer.stop();

	std::cout << result << "\t" << timer.getElapsedTimeInSeconds() << std::endl;
}



static void mainBefore(Arguments& arguments)
{
	auto R = RelationGenerator::generateUniform(1'000'000, 1, 19, 1, 100'000, 123123);
	auto S = RelationGenerator::generateUniform(1'000'000, 1, 19, 1, 100'000, 123);

	Index indexR(R);
	Index indexS(S);

	R.setIndex(indexR);
	S.setIndex(indexS);


	std::string implementation = arguments.getCurrentArgAndSkipIt("Implementation (normal or inlined)");

	bool isInlined = implementation == "inlined";

	for (size_t i = 1; i < 20; i++)
	{
		std::cout << i << '\t';

		if (isInlined)
		{
			timer([&]
			{
				return beforeJoinInlined(R, S, 3);
			});
		}
		else
		{
			timer([&]
			{
				Timestamp result = 0;
				beforeJoin(R, S, 3, [&] (const Tuple& r, const Tuple& s) noexcept
				{
					result += r.start + s.end;
				});
				return result;
			});
		}
	}
}


