#pragma once

#include "Util.h"


template<typename T>
class FastStack
{
private:
	T* elements = nullptr;
	T* end = 0;
	T* limit = 0;

public:
	FastStack()
	{
	}

	FastStack(size_t capacity)
	{
		reserve(capacity);
	}


	void reserve(size_t capacity)
	{
		assert(!elements);
		elements = array_malloc<T>(capacity),
		limit = elements + capacity;
	}


	~FastStack()
	{
		std::free(elements);
	}


	void push(T element)
	{
		if (end >= limit)
		{
			std::fprintf(stderr, "FastStack overflow\n");
			std::abort();
		}

		*end = element;
		end++;
	}

	T pop()
	{
		assert(end > elements);
		end--;
		return *end;
	}


	void clear() noexcept
	{
		end = elements;
	}


	size_t size() const noexcept
	{
		return end - elements;
	}


	bool empty() const noexcept
	{
		return end == elements;
	}


	FastStack& operator=(const FastStack&) = delete;
	FastStack(const FastStack&) = delete;

};



