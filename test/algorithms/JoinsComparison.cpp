#include <gtest/gtest.h>
#include <model/RelationGenerator.h>
#include "algorithms/IEJoin.h"
#include "algorithms/Joins.h"
#include "algorithms/LMJoins.h"
#include "util/Timer.h"


using testing::Test;


static Relation R;
static Relation S;

static Index indexR;
static Index indexS;


static Timestamp result;


static auto consumer = [] (const Tuple& r, const Tuple& s)
{
	result += r.start + s.end;
};



class JoinsComparison : public Test
{
protected:
	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
	static void SetUpTestCase()
	{
		R = RelationGenerator::generateUniform(10'000, 1, 1000, 1, 10'000, 5904595);
		S = RelationGenerator::generateUniform(10'000, 1, 1000, 1, 10'000, 58534);

		R.sort();
		S.sort();

		indexR.buildFor(R);
		indexS.buildFor(S);

		R.setIndex(indexR);
		S.setIndex(indexS);
	}
	#pragma clang diagnostic pop

//	virtual void TearDown()
//	{
//		result = 0;
//	}
};



TEST_F(JoinsComparison, reverseDuringStrict)
{
	Timer t;

//	result = 0;
//	for (const auto& r : R)
//	{
//		for (const auto& s : S)
//		{
//			if (r.start < s.start && s.end < r.end)
//				consumer(r, s);
//		}
//	}
//	auto naiveResult = result;
//	t.stopAndPrint();
//	std::cout << " " << result << "\n";


	result = 0;
	reverseDuringStrictJoin(R, S, consumer);
	auto normalResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	t.start();
	result = 0;
	leungMuntzReverseDuringStrictJoin(R, S, consumer);
	auto lmResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";



	EXPECT_EQ(normalResult, lmResult);
}




TEST_F(JoinsComparison, startPrecedingStrict)
{
	Timer t;

	result = 0;
	startPrecedingStrictJoin(R, S, consumer);
	auto normalResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	t.start();
	result = 0;
	leungMuntzStartPrecedingStrictJoin(R, S, consumer);
	auto lmResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	t.start();
	result = 0;
	ieJoinStartPrecedingStrictJoin(R, S, consumer);
	auto ieResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	EXPECT_EQ(normalResult, lmResult);
	EXPECT_EQ(normalResult, ieResult);
}





TEST_F(JoinsComparison, leftOverlapStrict)
{
	Timer t;

	result = 0;
	leftOverlapStrictJoin(R, S, consumer);
	auto normalResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	t.start();
	result = 0;
	leungMuntzLeftOverlapStrictJoin(R, S, consumer);
	auto lmResult = result;
	t.stopAndPrint();
	std::cout << " " << result << "\n";

	EXPECT_EQ(normalResult, lmResult);
}



