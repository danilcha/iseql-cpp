#pragma once

#include "../model/Relation.h"


struct EmptyPredicate
{
	bool operator()(const Tuple& r, const Tuple& s)
	{
		return true;
	}
};


template <typename Predicate = EmptyPredicate>
class BasicStartPreceding
{
public:
	static void run(const Relation& r, const Relation& s, const Predicate& predicate = Predicate())
	{

	}
};

