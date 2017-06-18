#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>


class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;

	Clock::time_point start_time, stop_time;

public:
	Timer()
	{
		start();
	}

	void start()
	{
		start_time = Clock::now();
	}


	double getElapsedTimeInSeconds()
	{
		return std::chrono::duration<double>(stop_time - start_time).count();
	}


	double stop()
	{
		stop_time = Clock::now();
		return getElapsedTimeInSeconds();
	}




	void print()
	{
		std::cout << std::fixed << std::setprecision(3) << getElapsedTimeInSeconds();
	}


	double stopAndPrint()
	{
		auto result = stop();
		print();
		return result;
	}
};
