#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include "model/Relation.h"
#include "util/String.h"


class RelationGenerator
{
private:
	typedef std::minstd_rand  RandomEngine;

public:
	typedef RandomEngine::result_type SeedType;



	static Relation generateUniform(size_t n, Timestamp durationMin, Timestamp durationMax, Timestamp domainMin, Timestamp domainMax, SeedType seed = RandomEngine::default_seed)
	{
		Relation result;
		result.reserve(n);

		assert(durationMin >= 1);
		assert(durationMin <= durationMax);
		assert(durationMax <= domainMax - domainMin + 1);
		assert(domainMin <= domainMax);

		typedef std::uniform_int_distribution<Timestamp> Distribution;

		RandomEngine generator(seed);

		for (size_t i = 0; i < n; i++)
		{
			Timestamp duration = Distribution{durationMin, durationMax}(generator);
			Timestamp begin    = Distribution{domainMin, domainMax - duration + 1}(generator);
			Timestamp end      = begin + duration;
			result.emplace_back(begin, end);
		}

		return result;
	}






//	template <typename OffsetDistribution, typename DurationDistribution>
//	static Relation generate(size_t n, OffsetDistribution& offsetDistribution,
//		DurationDistribution& durationDistribution, SeedType seed = RandomEngine::default_seed)
//	{
//		Relation result;
//		result.reserve(n);
//
//		RandomEngine generator(seed);
//
//		for (size_t i = 0; i < n; i++)
//		{
//			Timestamp duration = durationDistribution(generator);
//			Timestamp begin    = offsetDistribution(generator);
//			Timestamp end      = begin + duration;
//			result.emplace_back(begin, end);
//		}
//
//		return result;
//	}







	static Relation generateExponential(size_t n, double lambda, Timestamp domainMin, Timestamp domainMax, SeedType seed = RandomEngine::default_seed)
	{
		Relation result;
		result.reserve(n);

		assert(domainMin <= domainMax);



		std::exponential_distribution<double> lengthDistribution(lambda);
		typedef std::uniform_int_distribution<Timestamp> OffsetDistribution;

		RandomEngine generator(seed);

//		Tuple::Value uCharPayload = 0;
		for (size_t i = 0; i < n; i++)
		{
			double length;
			do
			{
				length = lengthDistribution(generator);
			}
			while (length >= domainMax - domainMin + 1); // length in [0, domainWidth)

			Timestamp beginAndEndDifference = (Timestamp) length;

			Timestamp begin    = OffsetDistribution{domainMin, domainMax - beginAndEndDifference}(generator);
			Timestamp end      = begin + beginAndEndDifference;
			result.emplace_back(begin, end + 1, i);
//			result.back().setValue(uCharPayload);
//			++uCharPayload;
		}

		return result;
	}





	static Relation loadFromClosedIntervalFile(const char* filename)
	{
		std::ifstream in(filename);
		if (!in)
		{
			std::cerr << "Cannot open file " << filename << std::endl;
			exit(1);
		}

		Relation result;
		Timestamp start, end;
//		Tuple::Value uCharPayload = 0;
		while (in >> start >> end)
		{
			assert(start <= end);
			result.emplace_back(start, end + 1);
//			result.back().setValue(uCharPayload);
//			++uCharPayload;
		}
		return result;
	}




	static Relation loadFromDASATemperatureFile(const char* filename)
	{
		std::ifstream in(filename);
		if (!in)
		{
			std::cerr << "Cannot open file " << filename << std::endl;
			exit(1);
		}

		Relation result;
		Timestamp start, end;
		double value;

		while (in >> start >> end >> value)
		{
			assert(start <= end);
			int intValue = (int) ((value + 100) * 1000);
			result.emplace_back(start, end, intValue);
		}
		return result;
	}


	// echo 'copy (select EXTRACT(EPOCH FROM measured_at)::INT,
	// EXTRACT(EPOCH FROM measured_at)::INT + 300, measured_value from measurements
	// where active and value_type_id = 1 order by measured_at) to stdout' | psql > asd.txt
	// static Relation loadFrom



	static Relation loadFromBASFFile(const char* filename)
	{
		std::ifstream in(filename);
		if (!in)
		{
			std::cerr << "Cannot open file " << filename << std::endl;
			exit(1);
		}

		int columnCount;
		in >> columnCount;
		for (int i = 0; i < columnCount; i++) // skip header
		{
			std::string s;
			in >> s >> s;
		}


		Relation result;
		std::string line;
		std::vector<std::string> fields;
		while (true)
		{
			in >> line;
			if (line[0] == '-')
				break;

			split(line, '|', fields);

			auto start = std::stoi(fields.at(1));
			auto end   = std::stoi(fields.at(2)) + 1;
			auto value = std::stoi(fields.at(5)); // nolines

			result.emplace_back(start, end, value);
		}

		return result;
	}
};

