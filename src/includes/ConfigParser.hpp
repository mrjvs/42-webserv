#pragma once

#include <vector>

#include "Server.hpp"

class ConfigParser
{
	public:
		ConfigParser(const char* path, std::vector<Server*>& servers);
		ConfigParser(const ConfigParser& rhs) {
			(void)rhs;
		}
		~ConfigParser() {}

		ConfigParser& operator=(const ConfigParser& rhs) {
			(void)rhs;
			return *this;
		}
};
