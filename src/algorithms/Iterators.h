#pragma once

#include "model/Index.h"



class Iterator
{
protected:
	Index::const_iterator it;
	Index::const_iterator end;

public:
	Iterator(const Index& index) noexcept
	:
		it(index.begin()),
		end(index.end())
	{
	}


	bool next() noexcept
	{
		++it;
		return it != end;
	}


	Endpoint getEndpoint() const noexcept
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

public:
	FilteringIterator(Iterator iterator, Endpoint::Type type) noexcept
	:
		iterator(iterator),
		type(type)
	{
		while (isInvalid())
			this->iterator.next();
	}

	bool next() noexcept
	{
		bool result;
		do
		{
			result = iterator.next();
		}
		while (result && isInvalid());

		return result;
	}

	bool isInvalid() const noexcept
	{
		return getEndpoint().getType() != type;
	}


	Endpoint getEndpoint() const noexcept
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

	bool next() noexcept
	{
		return iterator.next();
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
	Iterator1 iterator1;
	Iterator2 iterator2;
	Endpoint endpoint;

public:
	MergingIterator(Iterator1 iterator1, Iterator2 iterator2) noexcept
	:
		iterator1(iterator1),
		iterator2(iterator2)
	{
		next();
	}


	bool next() noexcept
	{
		if (iterator1.getEndpoint() < iterator2.getEndpoint())
		{
			endpoint = iterator1.getEndpoint();
			return iterator1.next();
		}
		else
		{
			endpoint = iterator2.getEndpoint();
			return iterator2.next();
		}
	}

	Endpoint getEndpoint() const noexcept
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



