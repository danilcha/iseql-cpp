#pragma once

#include <vector>
#include "model/Tuple.h"



class Index;



class Relation : public std::vector<Tuple>
{
private:
	const Index* index;



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




};

