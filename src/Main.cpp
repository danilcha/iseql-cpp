#include <iostream>
#include "algorithms/Joins.h"




int main()
{
	std::cout << "ISEQL    ";
	std::cout << sizeof(size_t) * 8 << "-bit    ";
	std::cout << "Compiled on " __DATE__ " " __TIME__ "    ";
	std::cout << "Tuple size " << sizeof(Tuple) << " bytes    ";
	#ifdef NDEBUG
		std::cout << "Release" << std::endl;
	#else
		std::cout << "Debug" << std::endl;
	#endif

	Relation R = {
		{1,  5, 1},
		{1, 10, 2},
		{7, 11, 3},
		{9, 10, 4},
	};
	Relation S = {
		{2,  2, 1},
		{3, 12, 2},
		{4,  5, 3},
		{5,  6, 4},
		{8,  9, 5},
	};

	Index indexR; indexR.buildFor(R);
	Index indexS; indexS.buildFor(R);

	R.setIndex(indexR);
	S.setIndex(indexS);


	startPrecedingJoin(R, S, [] (const Tuple& r, const Tuple& s)
	{
		std::cout << r << s << std::endl;
	});
}