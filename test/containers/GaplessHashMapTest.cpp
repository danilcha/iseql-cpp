#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "containers/GaplessHashMap.h"


using testing::UnorderedElementsAre;


TEST(Containers, GaplessHashMap)
{
	GaplessHashMap<int, int> map(10);

	EXPECT_THAT(map, UnorderedElementsAre());

	map.insert(0, 10);

	EXPECT_THAT(map, UnorderedElementsAre(10));

	map.insert(1, 11);
	map.insert(2, 12);
	map.insert(5, 15);

	EXPECT_THAT(map, UnorderedElementsAre(10, 11, 12, 15));

	map.erase(1);
	map.erase(5);

	EXPECT_THAT(map, UnorderedElementsAre(10, 12));

	map.erase(0);
	map.erase(2);

	EXPECT_THAT(map, UnorderedElementsAre());

	map.insert(1, 21);
	map.insert(2, 22);
	map.insert(5, 25);

	EXPECT_THAT(map, UnorderedElementsAre(21, 22, 25));
}


