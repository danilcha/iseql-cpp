#pragma once

#include "FastStack.h"


template<typename T>
class FastHeap
{
private:
	using Index = unsigned;

	FastStack<Index> freed;
	T* elements = nullptr;
	size_t dirtyElementCount;

	#ifndef NDEBUG
	size_t capacity;
	#endif



//	void* elements_free_pointer = nullptr;



public:
	FastHeap()
	{

	}

	FastHeap(size_t size) noexcept
	{
		reserve(size);
	}



	void reserve(size_t size) noexcept
	{
		#ifndef NDEBUG
		capacity = size;
		#endif

		freed.reserve(size);
		clear();

		assert(!elements);
		elements = array_malloc<T>(size);
//		elements_free_pointer = elements;
//		static constexpr size_t alignment = 64;
//		elements_free_pointer = std::malloc(size * sizeof(T) + alignment);
//		auto pointer = reinterpret_cast<uintptr_t>(elements_free_pointer);
//		pointer += alignment - pointer % alignment;
//		elements = reinterpret_cast<T*>(pointer);
	}



	~FastHeap() noexcept
	{
//		std::free(elements_free_pointer);
		std::free(elements);
	}


	T* alloc() noexcept
	{
		if (freed.empty())
		{
			assert(dirtyElementCount < capacity);
			auto result = elements + dirtyElementCount;
			dirtyElementCount++;
			return result;
		}
		else
			return elements + freed.pop();
	}


	void free(T* element) noexcept
	{
		auto index = size_t(element - elements);
		if (index == dirtyElementCount - 1)
			dirtyElementCount--;
		else
			freed.push(Index(index));
	}

	void clear() noexcept
	{
		freed.clear();
		dirtyElementCount = 0;
	}


	FastHeap& operator=(const FastHeap&) = delete;
	FastHeap(const FastHeap&) = delete;
};



