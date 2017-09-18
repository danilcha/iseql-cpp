#include <set>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "algorithms/Joins.h"
#include "algorithms/LeungMuntzJoins.h"


using testing::Test;
using testing::Pair;
using testing::UnorderedElementsAre;



static Relation R = {
	{1,  5, 1},
	{1, 10, 2},
	{7, 11, 3},
	{9, 10, 4},
};

static Relation S = {
	{2,  2, 1},
	{3, 12, 2},
	{4,  5, 3},
	{5,  6, 4},
	{8,  9, 5},
};


static Index indexR(R);
static Index indexS(S);

static std::set<std::pair<int, int>> result;


static auto consumer = [] (const Tuple& r, const Tuple& s)
{
	result.emplace(r.getId(), s.getId());
};



class Joins : public Test
{
protected:
	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
	static void SetUpTestCase()
	{
		R.setIndex(indexR);
		S.setIndex(indexS);
	}
	#pragma clang diagnostic pop

	virtual void TearDown()
	{
//		for (const auto& item : result)
//			std::cout << item.first << ' ' << item.second << std::endl;

		result.clear();
	}
};



TEST_F(Joins, startPreceding)
{
	startPrecedingJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(1, 2),
		Pair(1, 3),
		Pair(1, 4),
		Pair(2, 1),
		Pair(2, 2),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5)
	));
}



TEST_F(Joins, startPrecedingStrict)
{
	startPrecedingStrictJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(1, 2),
		Pair(1, 3),
		Pair(1, 4),
		Pair(2, 1),
		Pair(2, 2),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5)
	));
}



TEST_F(Joins, reverseStartPreceding)
{
	reverseStartPrecedingJoin(S, R, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(2, 1),
		Pair(3, 1),
		Pair(4, 1),
		Pair(1, 2),
		Pair(2, 2),
		Pair(3, 2),
		Pair(4, 2),
		Pair(5, 2),
		Pair(5, 3)
	));
}



TEST_F(Joins, startPrecedingWithDelta)
{
	startPrecedingJoin(R, S, 1, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(2, 1),
		Pair(3, 5)
	));
}



TEST_F(Joins, endFollowing)
{
	endFollowingJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(1, 3),
		Pair(2, 1),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5),
		Pair(4, 5)
	));
}



TEST_F(Joins, endFollowingStrict)
{
	endFollowingStrictJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(2, 1),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5),
		Pair(4, 5)
	));
}



TEST_F(Joins, reverseEndFollowing)
{
	reverseEndFollowingJoin(S, R, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(3, 1),
		Pair(1, 2),
		Pair(3, 2),
		Pair(4, 2),
		Pair(5, 2),
		Pair(5, 3),
		Pair(5, 4)
	));
}



TEST_F(Joins, endFollowingWithEpsilon)
{
	endFollowingJoin(R, S, 1, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 3),
		Pair(2, 5),
		Pair(4, 5)
	));
}



TEST_F(Joins, leftOverlap)
{
	leftOverlapJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 2),
		Pair(1, 3),
		Pair(1, 4),
		Pair(2, 2)
	));
}



TEST_F(Joins, leftOverlapWithDelta)
{
	leftOverlapJoin(R, S, 2, UNBOUND, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 2),
		Pair(2, 2)
	));
}



TEST_F(Joins, rightOverlap)
{
	rightOverlapJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(4, 5)
	));
}



TEST_F(Joins, rightOverlapWithEpsilon)
{
	rightOverlapJoin(R, S, UNBOUND, 0, consumer);

	EXPECT_THAT(result, UnorderedElementsAre());
}



TEST_F(Joins, during)
{
	duringJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(3, 2),
		Pair(4, 2)
	));
}





TEST_F(Joins, duringWithDelta)
{
	duringJoin(R, S, 4, UNBOUND, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(3, 2)
	));
}



TEST_F(Joins, reverseDuring)
{
	reverseDuringJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(1, 3),
		Pair(2, 1),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5)
	));
}


TEST_F(Joins, reverseDuringStrict)
{
	reverseDuringStrictJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(2, 1),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5)
	));
}


TEST_F(Joins, reverseDuringStrictLM)
{
	leungMuntzReverseDuringStrictJoin(R, S, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(2, 1),
		Pair(2, 3),
		Pair(2, 4),
		Pair(2, 5),
		Pair(3, 5)
	));
}


TEST_F(Joins, reverseDuringWithDeltaAndEpsilon)
{
	reverseDuringJoin(R, S, 4, 3, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 1),
		Pair(1, 3),
		Pair(3, 5)
	));
}



TEST_F(Joins, before)
{
	beforeJoin(R, S, 3, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(1, 5)
	));
}



TEST_F(Joins, after)
{
	afterJoin(R, S, 2, consumer);

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(3, 3),
		Pair(3, 4)
	));
}



