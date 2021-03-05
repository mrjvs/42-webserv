#pragma once

#include <vector>

#include "Server.hpp"

namespace config
{
	class ConfigParser
	{
		public:
			ConfigParser(const char* path, std::vector<Server*>& servers);
			~ConfigParser() {}
	};
}

