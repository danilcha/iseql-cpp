#include <iostream>
#include "algorithms/BasicStartPreceeding.h"
#include "model/Interval.h"
#include "model/Tuple.h"
#include "model/Relation.h"

int main()
{
	std::cout << "Hello, World!" << std::endl;

	Relation r, s;

	BasicStartPreceding<>::run(r, s);
}
