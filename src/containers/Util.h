#pragma once

#include <cstdlib>
#include <cstdio>



template<typename T>
void check_malloc_result(T* result, size_t size = 1) noexcept
{
	if (!result)
	{
		std::fprintf(stderr, "Failed to allocate %zu object(s) of size %zu\n", size, sizeof(T));
		std::abort();
	}
}



template<typename T>
T* array_calloc(size_t size) noexcept
{
	auto result = static_cast<T*>(std::calloc(size, sizeof(T)));
	check_malloc_result(result, size);
	return result;
}


template<typename T>
T* array_malloc(size_t size) noexcept
{
	auto result = static_cast<T*>(std::malloc(size * sizeof(T)));
	check_malloc_result(result, size);
	return result;
}

template<typename T>
T* array_realloc(T* array, size_t size) noexcept
{
	auto result = static_cast<T*>(std::realloc(array, size * sizeof(T)));
	check_malloc_result(result, size);
	return result;
}


template <typename T>
T next_power_of_two(T x)
{
	T result = 2;
	while (result < x)
		result <<= 1;
	return result;
}


//template <typename T>
//T ceil_div(T a, T b)
//{
//	static_assert(std::is_unsigned<T>::value, "ceil_div works only with unsigned types");
//	return (a + b - 1) / b;
//}
