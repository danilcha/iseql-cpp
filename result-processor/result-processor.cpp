#include <iostream>
#include "FileUtil.h"
#include "TableIO.h"













static void main2()
{
	std::string input  = "results/cyprus";
	std::string output = "../iseql-article/data";


	{
		Table tableGEDMatrix = TableIO::loadTableFromDir(input + "/GEDMatrix");

		for (const std::string& length : {"10", "100", "1000"})
		{
			Table table = tableGEDMatrix;

			table.rename("col" + length, "matrix-col");
			table.rename("row" + length, "matrix-row");
			table.rename("ed"  + length, "ed");

			TableIO::saveTable(table, output + "/GEDMatrix-L-" + length + ".txt");
		}
	}


	{
		Table tableTopIndex = TableIO::loadTableFromDir(input + "/TopIndex");


		for (auto p : {"1", "5", "10"})
		{
			Table table = tableTopIndex.where("p", p).pivot("k", "index", "c");

			TableIO::saveTable(table, output + "/TopIndex-P-" + p + ".txt");
		}
	}

	{
		Table tableDeltaTopIndex = TableIO::loadTableFromDir(input + "/DeltaTopIndex");

		for (auto n : {"115", "58", "29"})
		{
			std::cout << 4 << std::endl;
			Table compression = tableDeltaTopIndex.where("n", n).pivot("p", "compression", "k");
			TableIO::saveTable(compression, output + "/Compression-N-" + n + ".txt");
		}
		std::cout << 5 << std::endl;


		tableDeltaTopIndex.filter("n", "115");

		for (auto p : {"1152", "576", "288"})
		{
			Table query = tableDeltaTopIndex.where("p", p).unpivot("query", "query", "l");

			query = query.pivot("k", "query", "l");

			TableIO::saveTable(query, output + "/DeltaQuery-P-" + p + ".txt");
		}
	}
}



int main()
{
	try
	{
		main2();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Unknown exception occurred" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
