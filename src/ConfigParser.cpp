#include "includes/ConfigParser.hpp"
#include "includes/getLine.hpp"
#include "includes/Server.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>

namespace config
{
	ConfigParser::ConfigParser(const char* path, std::vector<Server*>& servers)
	{
		int fd = -1;
		struct stat sb;
		std::string buf;
		
		if (path && stat(path, &sb) != -1)
			fd = open(path, O_RDONLY);
		if (fd < 0)
			throw std::runtime_error("fail to open config file");

		while (ft::getLine(fd, buf))
		{
			if (buf == "server {")
			{
				Server* tmp = new Server;
				tmp->parse(fd);
				servers.push_back(tmp);
			}
		}
	}
}
