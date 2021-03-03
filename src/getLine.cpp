#include "includes/getLine.hpp"

namespace ft
{
	int		getLine(int fd, std::string& line)
	{
		char tmp[BUFFER_SIZE + 1];
		int ret = 1;
		static std::map<int, std::string> buf;
		
		line.clear();
		while (ret)
		{
			line += buf[fd];
			if (line.find('\n') != std::string::npos)
			{
				buf[fd] = line.substr(line.find_first_of('\n') + 1, line.length() - line.find_first_of('\n') - 1);
				line = line.substr(0, line.find_first_of('\n'));
				return ret > 0;
			}
			buf[fd].clear();
			ret = read(fd, tmp, BUFFER_SIZE);
			tmp[ret] = '\0';
			buf[fd].assign(tmp);
		}
		return (0);
	}
}
