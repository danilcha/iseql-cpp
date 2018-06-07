#pragma once

#include <string>
#include <sstream>
#include <vector>




static void split(const std::string& s, char delimiter, std::vector<std::string>& result)
{
	std::istringstream ss(s);

	size_t i = 0;
	while (i < result.size() && std::getline(ss, result[i], delimiter))
		i++;

	if (i < result.size())
	{
		result.resize(i);
		return;
	}

	if (ss.eof())
		return;

	std::string item;
	while (std::getline(ss, item, delimiter))
		result.emplace_back(std::move(item));
}


inline static std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> result;
	split(s, delimiter, result);
	return result;
}
