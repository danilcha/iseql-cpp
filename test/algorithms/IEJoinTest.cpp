#include <set>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "algorithms/IEJoin.h"

using testing::Pair;
using testing::UnorderedElementsAre;


static Relation R = {
	{140, 1009, 1},
	{100, 1012, 2},
	{ 90, 1005, 3},
};

static Relation S = {
	{100, 1006, 1},
	{140, 1011, 2},
	{ 80, 1010, 3},
	{ 90, 1005, 4},
};




TEST(IEJoinTests, BasicTest)
{
	IEJoinOperator2<GetIntervalStart, JoinCondition::LessThan,    GetIntervalStart,
	                GetIntervalEnd,   JoinCondition::GreaterThan, GetIntervalEnd> join{R, S};

	std::vector<std::pair<int, int>> result;

	join.join([&] (const Tuple& r, const Tuple& s)
	{
		result.emplace_back(r.getId(), s.getId());
//		std::cout << r.getId() << ' ' << s.getId() << std::endl;
	});

	EXPECT_THAT(result, UnorderedElementsAre(
		Pair(2, 2)
	));

}

