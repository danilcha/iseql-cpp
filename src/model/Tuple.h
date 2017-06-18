#pragma once

#include "model/Interval.h"



class Tuple : public Interval
{
private:
	int id;


public:
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
		return out << tuple.id << static_cast<const Interval&>(tuple);
	}


	int getId() const noexcept
	{
		return id;
	}

};

