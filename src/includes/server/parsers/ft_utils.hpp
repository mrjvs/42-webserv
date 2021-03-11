#pragma once

#include <string>
#include <vector>

namespace ft
{
	size_t							stoi(const std::string& str);
	
	size_t							atoi(const char str);

	std::vector<std::string>		split(const std::string& str, const std::string& del);

	size_t							countWS(const std::string& str);
}
