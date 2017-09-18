#pragma once

#include <vector>
#include <random>
#include "model/Tuple.h"



class Index;



class Relation : public std::vector<Tuple>
{
private:
	const Index* index;

	typedef std::default_random_engine RandomEngine;
	typedef RandomEngine::result_type SeedType;


public:
	Relation() { }

	Relation(std::initializer_list<Tuple> list)
	:
		vector(list)
	{
	}


	void setIndex(const Index& index) noexcept
	{
		this->index = &index;
	}


	const Index& getIndex() const noexcept
	{
		return *this->index;
	}


	void sort()
	{
		std::sort(begin(), end());
	}


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
			Timestamp end      = begin + duration - 1;
			result.emplace_back(begin, end);
		}

		return result;
	}



	template <typename OffsetDistribution, typename DurationDistribution>
	static Relation generate(size_t n, OffsetDistribution& offsetDistribution,
		DurationDistribution& durationDistribution, SeedType seed = RandomEngine::default_seed)
	{
		Relation result;
		result.reserve(n);

		RandomEngine generator(seed);

		for (size_t i = 0; i < n; i++)
		{
			Timestamp duration = durationDistribution(generator);
			Timestamp begin    = offsetDistribution(generator);
			Timestamp end      = begin + duration - 1;
			result.emplace_back(begin, end);
		}

		return result;
	}
};

