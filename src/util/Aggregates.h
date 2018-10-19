#pragma once

#include <limits>
#include <algorithm>


template <typename T>
struct Aggregate
{
	T max = std::numeric_limits<T>::min();
	T min = std::numeric_limits<T>::max();
	size_t count = 0;
	long long sum = 0;

	void add(T value)
	{
		max = std::max(max, value);
		min = std::min(min, value);
		sum += (long long) value;
		count++;
	}


	double getAvg()
	{
		return (double) sum / count;
	}
};
