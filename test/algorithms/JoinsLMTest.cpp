#include <gtest/gtest.h>
#include "algorithms/Joins.h"
#include "algorithms/LeungMuntzJoins.h"
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



class JoinsLM : public Test
{
protected:
	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
	static void SetUpTestCase()
	{
		R = Relation::generateUniform(10, 10000, 10000, 1, 10000, 5904595);
		S = Relation::generateUniform(10, 10000, 10000, 1, 10000, 58534);

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



TEST_F(JoinsLM, reverseDuringStrict)
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



