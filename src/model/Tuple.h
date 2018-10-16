#pragma once

#include "model/Interval.h"



class Tuple : public Interval
{
public:
	int id;

//	char data[32 - 12];

public:
	Tuple() noexcept {}

	Tuple(Timestamp start, Timestamp end) noexcept
	:
		Interval(start, end)
	{
	}


	Tuple(Timestamp start, Timestamp end, int id) noexcept
	:
		Interval(start, end),
		id(id)
	{

	}


	friend std::ostream& operator << (std::ostream &out, const Tuple& tuple)
	{
		return out << static_cast<const Interval&>(tuple) << ' ' << tuple.id;
	}


	int getId() const noexcept
	{
		return id;
	}

};

