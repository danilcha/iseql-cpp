#pragma once

#include <util/Experiments.h>
#include <algorithms/JoinsInlined.h>



static void mainBefore()
{
	auto R = Relation::generateUniform(1'000'000, 1, 19, 1, 100'000, 123123);
	auto S = Relation::generateUniform(1'000'000, 1, 19, 1, 100'000, 123);


	Experiments experiments;

	Index indexR;
	Index indexS;

	experiments.prepare("index", "", [&]
	{
		indexR.buildFor(R);
		indexS.buildFor(S);

		R.setIndex(indexR);
		S.setIndex(indexS);
	});


	experiments.experiment("before", "index", [&]
	{
		Timestamp result = 0;
		beforeJoin(R, S, 3, [&] (const Tuple& r, const Tuple& s) noexcept
		{
			result += r.start + s.end;
		});
		return result;
	});

	experiments.experiment("before_inline", "index", [&]
	{
		return beforeJoinInlined(R, S, 3);
	});
}


