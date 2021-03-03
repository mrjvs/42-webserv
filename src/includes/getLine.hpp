#pragma once

# include <string>
# include <stdlib.h>
# include <map>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 8
# endif

namespace ft
{
	int		getLine(int fd, std::string& line);
}
