#pragma once

#include <limits>
#include <cassert>
#include <ostream>

using Timestamp = int;


class Interval
{
public:
	Timestamp start; // inclusive
	Timestamp end;   // exclusive

	static constexpr auto NEGATIVE_INFINITY = std::numeric_limits<Timestamp>::min();
	static constexpr auto POSITIVE_INFINITY = std::numeric_limits<Timestamp>::max();

public:
	Interval(Timestamp start, Timestamp end) noexcept
	:
		start(start),
		end(end)
	{
		assert(start < end);
	}


	static Interval WHOLE_TIMELIME()
	{
		return {NEGATIVE_INFINITY, POSITIVE_INFINITY};
	}


	bool operator < (const Interval& rhs) const noexcept
	{
		return start < rhs.start;
	}


	inline bool overlaps(const Interval& other) const noexcept
	{
		return start < other.end && other.start < end;
	}


	inline bool covers(const Interval& other) const noexcept
	{
		return start <= other.start && end >= other.end;
	}


	friend void swap(Interval& a, Interval& b) noexcept
	{
		using std::swap;
		swap(a.start, b.start);
		swap(a.end  , b.end  );
	}


	auto inclusiveEnd() const noexcept
	{
		return end - 1;
	}


	auto getLength() const noexcept
	{
		return end - start;
	}


	friend std::ostream& operator << (std::ostream &out, const Interval& self)
	{
		return out << '['
				<< (self.start == NEGATIVE_INFINITY ? "inf" : std::to_string(self.start)) << ", "
				<< (self.end   == POSITIVE_INFINITY ? "inf" : std::to_string(self.end  )) << ')';
	}
};



//TEST_CASE()
//{
//	REQUIRE      (Interval(3, 7).overlaps(Interval(3, 7)));
//	REQUIRE      (Interval(1, 4).overlaps(Interval(3, 7)));
//	REQUIRE_FALSE(Interval(1, 3).overlaps(Interval(3, 7)));
//	REQUIRE      (Interval(6, 9).overlaps(Interval(3, 7)));
//	REQUIRE_FALSE(Interval(7, 9).overlaps(Interval(3, 7)));
//
//	// The same as above, just swapped intervals
//	REQUIRE      (Interval(3, 7).overlaps(Interval(3, 7)));
//	REQUIRE      (Interval(3, 7).overlaps(Interval(1, 4)));
//	REQUIRE_FALSE(Interval(3, 7).overlaps(Interval(1, 3)));
//	REQUIRE      (Interval(3, 7).overlaps(Interval(6, 9)));
//	REQUIRE_FALSE(Interval(3, 7).overlaps(Interval(7, 9)));
//
//	REQUIRE      (Interval(3, 9) < Interval(4, 5));
//	REQUIRE_FALSE(Interval(4, 9) < Interval(4, 5));
//	REQUIRE_FALSE(Interval(5, 9) < Interval(4, 5));
//
//	REQUIRE      (Interval(1, 3).inclusiveEnd() == 2);
//
//	REQUIRE      (Interval(1, 3).covers(Interval(1, 3)));
//	REQUIRE      (Interval(1, 3).covers(Interval(2, 3)));
//	REQUIRE      (Interval(1, 3).covers(Interval(1, 2)));
//	REQUIRE_FALSE(Interval(1, 3).covers(Interval(0, 3)));
//	REQUIRE_FALSE(Interval(1, 3).covers(Interval(1, 4)));
//	REQUIRE_FALSE(Interval(1, 3).covers(Interval(0, 4)));
//}


