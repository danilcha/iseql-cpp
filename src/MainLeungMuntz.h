#pragma once

#include <iostream>
#include "algorithms/LeungMuntzJoins.h"




static void mainLeungMuntz(Arguments& arguments)
{
	auto R = Relation::generateUniform(1'000'000, 1, 19, 1, 100'000, 123123);
	auto S = Relation::generateUniform(1'000'000, 1, 19, 1, 100'000, 123);

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


