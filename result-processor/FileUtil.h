#pragma once

#include <boost/filesystem.hpp>
#include <boost/range.hpp>
#include <vector>
#include <string>


static std::vector<std::string> ls(const std::string& directory, const std::string& extension)
{
	std::vector<std::string> result;

	for (const auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator{directory}, {}))
	{
		if (boost::filesystem::is_regular_file(entry) && entry.path().extension() == extension)
			result.push_back(entry.path().string());
	}

	return result;
}




