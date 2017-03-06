#include "gtest/gtest.h"
#include "model/Interval.h"


TEST(Interval, Basic)
{
	EXPECT_TRUE ( Interval(3, 7).overlaps(Interval(3, 7)) );
	EXPECT_TRUE ( Interval(1, 3).overlaps(Interval(3, 7)) );
	EXPECT_FALSE( Interval(1, 2).overlaps(Interval(3, 7)) );
	EXPECT_TRUE ( Interval(7, 9).overlaps(Interval(3, 7)) );
	EXPECT_FALSE( Interval(8, 9).overlaps(Interval(3, 7)) );

	EXPECT_TRUE ( Interval(3, 7).overlaps(Interval(3, 7)) );
	EXPECT_TRUE ( Interval(3, 7).overlaps(Interval(1, 3)) );
	EXPECT_FALSE( Interval(3, 7).overlaps(Interval(1, 2)) );
	EXPECT_TRUE ( Interval(3, 7).overlaps(Interval(7, 9)) );
	EXPECT_FALSE( Interval(3, 7).overlaps(Interval(8, 9)) );

	EXPECT_TRUE ( Interval(3, 9) < Interval(4, 5)         );
	EXPECT_FALSE( Interval(4, 9) < Interval(4, 5)         );
	EXPECT_FALSE( Interval(5, 9) < Interval(4, 5)         );

	EXPECT_TRUE ( Interval(1, 3).covers(Interval(1, 3))   );
	EXPECT_TRUE ( Interval(1, 3).covers(Interval(2, 3))   );
	EXPECT_TRUE ( Interval(1, 3).covers(Interval(1, 2))   );
	EXPECT_FALSE( Interval(1, 3).covers(Interval(0, 3))   );
	EXPECT_FALSE( Interval(1, 3).covers(Interval(1, 4))   );
	EXPECT_FALSE( Interval(1, 3).covers(Interval(0, 4))   );
}
