#include "includes/ConfigParser.hpp"
#include "includes/ft_utils.hpp"
#include "includes/Location.hpp"
#include <iostream>

int	main(int argc, char** argv)
{
	std::vector<Server*> servers;

	try
	{
		if (argc != 2)
			ConfigParser parse("config_files/config1.conf", servers);
		else
			ConfigParser parse(argv[1], servers);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	std::cout << servers << std::endl;

	return (0);
}
