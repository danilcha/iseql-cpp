#include <gtest/gtest.h>
#include "model/Interval.h"


TEST(Interval, Basic)
{
	EXPECT_TRUE ( Interval(3,  8).overlaps(Interval(3, 8)) );
	EXPECT_TRUE ( Interval(1,  4).overlaps(Interval(3, 8)) );
	EXPECT_FALSE( Interval(1,  3).overlaps(Interval(3, 8)) );
	EXPECT_TRUE ( Interval(7, 10).overlaps(Interval(3, 8)) );
	EXPECT_FALSE( Interval(8, 10).overlaps(Interval(3, 8)) );

	EXPECT_TRUE ( Interval(3, 8).overlaps(Interval(3,  8)) );
	EXPECT_TRUE ( Interval(3, 8).overlaps(Interval(1,  4)) );
	EXPECT_FALSE( Interval(3, 8).overlaps(Interval(1,  3)) );
	EXPECT_TRUE ( Interval(3, 8).overlaps(Interval(7, 10)) );
	EXPECT_FALSE( Interval(3, 8).overlaps(Interval(8, 10)) );

	EXPECT_TRUE ( Interval(3, 10) < Interval(4, 6)         );
	EXPECT_FALSE( Interval(4, 10) < Interval(4, 6)         );
	EXPECT_FALSE( Interval(5, 10) < Interval(4, 6)         );

	EXPECT_TRUE ( Interval(1, 4).covers(Interval(1, 4))   );
	EXPECT_TRUE ( Interval(1, 4).covers(Interval(2, 4))   );
	EXPECT_TRUE ( Interval(1, 4).covers(Interval(1, 3))   );
	EXPECT_FALSE( Interval(1, 4).covers(Interval(0, 4))   );
	EXPECT_FALSE( Interval(1, 4).covers(Interval(1, 5))   );
	EXPECT_FALSE( Interval(1, 4).covers(Interval(0, 5))   );
}
