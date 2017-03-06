#pragma once

#include "Interval.h"


class Tuple : public Interval
{
public:
	Tuple(Timestamp start, Timestamp end) noexcept
	:
		Interval(start, end)
	{

	}

};

