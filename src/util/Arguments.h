#pragma once

#include <iostream>

class Arguments
{
private:
	const char** argv;
	size_t index = 1;


private:
	const char* getCurrentArgOrNull()
	{
		return argv[index];
	}

	void skipCurrentArg()
	{
		index++;
	}



public:
	Arguments(const char* argv[]) : argv(argv)
	{

	}


	bool isSwitchPresent(const char* switchName)
	{
		if (getCurrentArgOrNull() != nullptr && getCurrentArgOrNull() == std::string(switchName))
		{
			skipCurrentArg();
			return true;
		}
		else
			return false;
	}


	const char* getCurrentArgAndSkipIt(const char* description)
	{
		auto arg = getCurrentArgOrNull();
		if (!arg)
			error("Expected command-line argument: ", description);
		index++;
		return arg;
	}


	double getCurrentArgAsDoubleAndSkipIt(const char* description)
	{
		return strtod(getCurrentArgAndSkipIt(description), nullptr);
	}


	bool empty()
	{
		return argv[index] == nullptr;
	}



	[[noreturn]] void error(const char* reason, const std::string& reason2)
	{
		error(reason, reason2.c_str());
	}


	[[noreturn]] void error(const char* reason, const char* reason2)
	{
		std::cerr << reason << reason2 << std::endl;
		exit(1);
	}
};


