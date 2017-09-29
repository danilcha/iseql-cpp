#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "Table.h"



class TableIO
{
	using Row = Table::Row;

private:
	static Row split(std::string str)
	{
		std::istringstream in{str};

		return Row{std::istream_iterator<std::string>{in}, std::istream_iterator<std::string>{}};
	}


public:
	static void addRowFromFile(Table& table, const std::string& filename)
	{
		std::ifstream in;
		in.exceptions(std::ios::failbit | std::ios::badbit);
		in.open(filename);


		std::string line;

		while (std::getline(in, line) && line != "-----BEGIN RESULTS-----")
		{
			// skip line
		}

		std::getline(in, line);
		Row headers = split(line);

		std::getline(in, line);
		Row values = split(line);

		table.addRow(headers, values);
	}


	static void saveTable(const Table& table, const std::string& filename, int width = 15)
	{
		std::cout << "Saving table to " << filename << std::endl;

		std::ofstream out;
		out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		out.open(filename);

		out << std::left;

		for (const auto& column : table.getColumns())
			out << std::setw(width) << column << ' ';
		out << '\n';

		for (const Row& row : table.getRows())
		{
			for (const auto& value : row)
			{
				out << std::setw(width);
				if (value.empty())
					out << "nan";
				else
					out << value;
				out	<< ' ';
			}
			out << '\n';
		}
	}


	static Table loadTableFromDir(const std::string& dir)
	{
		Table table;

		std::cout << "Adding files from " << dir << std::endl;

		for (const std::string& file : ls(dir, ".txt"))
		{
			std::cout << "File found " << file << std::flush;
			try
			{
				addRowFromFile(table, file);
			}
			catch (const std::ios_base::failure& failure)
			{
				std::cout << ": " << failure.what();
			}
			std::cout << std::endl;
		}

		return table;
	}

};

