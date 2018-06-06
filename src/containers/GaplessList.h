#pragma once

#include <cstdint>
#include <vector>
#include "Util.h"





template<typename T>
class GaplessList
{
private:
	std::vector<T> elements;


public:
	using iterator       = typename std::vector<T>::iterator;
//	using const_iterator = std::vector<T>::const_iterator;


public:
	GaplessList()
	{
	}


	GaplessList(size_t capacity)
	{
		elements.reserve(capacity);
	}


	void insert(const T& element)
	{
		elements.push_back(element);
	}


	void erase(iterator it)
	{
		auto last = elements.end() - 1;
		if (it != last)
			*it = *last;
		elements.erase(last);
	}


	iterator begin() noexcept
	{
		return elements.begin();
	}


	iterator end() noexcept
	{
		return elements.end();
	}


	bool empty() const noexcept
	{
		return elements.empty();
	}

	size_t size() const noexcept
	{
		return elements.size();
	}
};

