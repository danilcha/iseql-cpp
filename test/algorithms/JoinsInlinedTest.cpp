#include <gtest/gtest.h>
#include <model/RelationGenerator.h>
#include "algorithms/Joins.h"
#include "algorithms/JoinsInlined.h"


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



class JoinsInlined : public Test
{
protected:
	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
	static void SetUpTestCase()
	#pragma clang diagnostic pop
	{
		R = RelationGenerator::generateUniform(1000, 1, 100, 1, 10000, 453565455);
		S = RelationGenerator::generateUniform(1000, 1, 100, 1, 10000, 585);

		indexR.buildFor(R);
		indexS.buildFor(S);

		R.setIndex(indexR);
		S.setIndex(indexS);
	}

//	virtual void TearDown()
//	{
//		result = 0;
//	}
};



TEST_F(JoinsInlined, before)
{
	for (Timestamp delta = 0; delta < 10; delta++)
	{
		result = 0;

		beforeJoin(R, S, delta, consumer);

		auto inlinedResult = beforeJoinInlined(R, S, delta);

		EXPECT_EQ(result, inlinedResult);
	}
}



