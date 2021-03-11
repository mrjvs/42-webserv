#include "server/parsers/ft_utils.hpp"

namespace ft
{
	size_t		stoi(const std::string& str)
	{
		size_t ret = 0;
		for (size_t i = 0; str[i]; ++i) {
			if (!std::isdigit(str[i]))
				return (0);
			ret = ret * 10 + (str[i] - '0');
		}
		return ret;
	}

	size_t		atoi(const char* str)
	{
		size_t ret = 0;
		for (size_t i = 0; str[i]; ++i) {
			if (!std::isdigit(str[i]))
				return (0);
			ret = ret * 10 + (str[i] - '0');
		}
		return ret;
	}

	std::vector<std::string>		split(const std::string& str, const std::string& del)
	{
		size_t start = 0, end = 0;
		std::vector<std::string> array;


		while (end != str.npos)
		{
			start = str.find_first_not_of(del, end);
			end = str.find_first_of(del, start);
			if (end != std::string::npos || start != std::string::npos)
				array.push_back(str.substr(start, end - start));
		}
		return array;
	}

	size_t		countWS(const std::string& str) {
		size_t spaces = 0;
		for (size_t i = 0; str[i]; ++i)
			if (str[i] == ' ')
				++spaces;
		return spaces;
	}

}
