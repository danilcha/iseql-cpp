#pragma once

#include "model/Index.h"



class Iterator
{
private:
	Index::const_iterator it;
	Index::const_iterator end;

public:
	Iterator(const Index& index) noexcept
	:
		it(index.begin()),
		end(index.end())
	{
	}


	void moveToNextEndpoint() noexcept
	{
		++it;
	}


	bool isFinished() const noexcept
	{
		return it == end;
	}


	const Endpoint& getEndpoint() const noexcept
	{
		return *it;
	}
};



template <typename Iterator>
class FilteringIterator
{
private:
	Iterator iterator;
	Endpoint::Type type;

	bool isWrongType() const noexcept
	{
		return getEndpoint().getType() != type;
	}

public:
	FilteringIterator(Iterator iterator, Endpoint::Type type) noexcept
	:
		iterator(iterator),
		type(type)
	{
		while (isWrongType())
			this->iterator.moveToNextEndpoint();
	}

	void moveToNextEndpoint() noexcept
	{
		do
		{
			iterator.moveToNextEndpoint();
		}
		while (!isFinished() && isWrongType());
	}


	bool isFinished() const noexcept
	{
		return iterator.isFinished();
	}


	const Endpoint& getEndpoint() const noexcept
	{
		return iterator.getEndpoint();
	}
};



template <typename Iterator>
class ShiftingIterator
{
private:
	Iterator iterator;
	Timestamp shiftArgument;

public:
	ShiftingIterator(Iterator iterator, Timestamp delta) noexcept
	:
		iterator(iterator),
		shiftArgument(Endpoint::calculateShiftArgument(delta))
	{
	}


	ShiftingIterator(Iterator iterator, Timestamp delta, Endpoint::Type fromType, Endpoint::Type toType) noexcept
	:
		iterator(iterator),
		shiftArgument(Endpoint::calculateShiftArgument(delta, fromType, toType))
	{
	}


	void moveToNextEndpoint() noexcept
	{
		iterator.moveToNextEndpoint();
	}


	bool isFinished() const noexcept
	{
		return iterator.isFinished();
	}


	Endpoint getEndpoint() const noexcept
	{
		return iterator.getEndpoint().shiftedBy(shiftArgument);
	}
};



template <typename Iterator1, typename Iterator2>
class MergingIterator
{
private:
	Iterator1 iterator1; // finishes always before the second iterator
	Iterator2 iterator2;
	Endpoint endpoint;

public:
	MergingIterator(Iterator1 iterator1, Iterator2 iterator2) noexcept
	:
		iterator1(iterator1),
		iterator2(iterator2)
	{
		assert(iterator1.getEndpoint() < iterator2.getEndpoint());

		moveToNextEndpoint();
	}


	void moveToNextEndpoint() noexcept
	{
		auto endpoint1 = iterator1.getEndpoint();
		auto endpoint2 = iterator2.getEndpoint();

		if (!iterator1.isFinished() && endpoint1 < endpoint2)
		{
			endpoint = endpoint1;
			iterator1.moveToNextEndpoint();
		}
		else
		{
			endpoint = endpoint2;
			iterator2.moveToNextEndpoint();
		}
	}


	bool isFinished() const noexcept
	{
		return iterator2.isFinished();
	}


	const Endpoint& getEndpoint() const noexcept
	{
		return endpoint;
	}
};



template <typename Iterator>
FilteringIterator<Iterator> makeFilteringIterator(Iterator iterator, Endpoint::Type type) noexcept
{
	return FilteringIterator<Iterator>(iterator, type);
}



static inline FilteringIterator<Iterator> makeFilteringIterator(const Index& index, Endpoint::Type type) noexcept
{
	return makeFilteringIterator(Iterator(index), type);
}



template <typename Iterator>
ShiftingIterator<Iterator> makeShiftingIterator(Iterator iterator, int delta) noexcept
{
	return ShiftingIterator<Iterator>(iterator, delta);
}



template <typename Iterator>
ShiftingIterator<Iterator> makeShiftingIterator(Iterator iterator, int delta, Endpoint::Type fromType, Endpoint::Type toType) noexcept
{
	return ShiftingIterator<Iterator>(iterator, delta, fromType, toType);
}



template <typename Iterator1, typename Iterator2>
MergingIterator<Iterator1, Iterator2> makeMergingIterator(Iterator1 iterator1, Iterator2 iterator2) noexcept
{
	return MergingIterator<Iterator1, Iterator2>(iterator1, iterator2);
}
