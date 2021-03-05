#include "includes/Server.hpp"
#include "includes/getLine.hpp"
#include "includes/ft_utils.hpp"
#include "includes/Location.hpp"

#include <iostream>

Server::Server() {}

void		Server::getKeyAndValues(int fd, std::string& line, std::string& key, std::vector<std::string>& values)
{
	std::vector<std::string> args = ft::split(line, " \t");
	key = args[0];
	if (key == "location")
	{
		values.assign(args.begin()+1, args.end());
		while (ft::getLine(fd, line) && line != "\t}")
			values.push_back(line);
	}
	else
		values.assign(args.begin()+1, args.end());
}

void		Server::parse(int fd)
{
	std::string line, key;
	std::vector<std::string> values;
	std::map<std::string, void(Server::*)(const std::vector<std::string>&)> set;

	set["port"] = &Server::setPort;
	set["host"] = &Server::setHost;
	set["server_name"] = &Server::setServer_name;
	set["error_page"] = &Server::setError_page;
	set["max_filesize"] = &Server::setMaxfilesize;
	set["location"] = &Server::setLocation;
	
	while (ft::getLine(fd, line))
	{
		if (line.empty())
			continue ;
		if (line == "}")
			break ;
		else if (line[0] != '\t')
			throw std::runtime_error("invalid");
		getKeyAndValues(fd, line, key, values);
		try {
			(this->*(set.at(key)))(values);
		}
		catch (const std::exception& e) {
			std::runtime_error("* unknown key in config file *");
			// std::cout << "* unknown key in config file *\n";
		}
	}
	if (!_port || !_maxfilesize || _host.empty() || _server_name.empty() || _error_page.empty())
		throw std::runtime_error("invalid");
}


void	Server::setLocation(const std::vector<std::string>& locSettings)
{
	Location* loc = new Location;
	std::map<std::string, void(Location::*)(const std::vector<std::string>&)> set;

	set["location"] = &Location::setPrefix;
	set["root"] = &Location::setRoot;
	set["allow_method"] = &Location::setAllow_method;
	set["autoindex"] = &Location::setAutoindex;
	set["index"] = &Location::setIndex;
	set["maxBody"] = &Location::setMaxBody;

	for (size_t i = 0; i < locSettings.size(); ++i) {
		std::vector<std::string> args = ft::split(locSettings[i], " \t\n");
		try {
			(loc->*(set.at(args[0])))(args);
		}
		catch (const std::exception& e) {
			std::runtime_error("* unknown key in config file *");
			// std::cout << "* unknown key in config file *\n";
		}
	}
	this->_locations.push_back(loc);
}

void	Server::setPort(const std::vector<std::string>& args)
{
	this->_port = ft::stoi(args[0]);
}

void	Server::setHost(const std::vector<std::string>& args)
{
	this->_host = args[0];
}

void	Server::setServer_name(const std::vector<std::string>& args)
{
	this->_server_name = args[0];
}

void	Server::setError_page(const std::vector<std::string>& args)
{
	this->_error_page = args[0];
}

void	Server::setMaxfilesize(const std::vector<std::string>& args)
{
	this->_maxfilesize = ft::stoi(args[0]);
}

size_t						Server::getPort() const { return this->_port; }
std::string					Server::getHost() const { return this->_host; }
std::string					Server::getServer_name() const { return this->_server_name; }
std::string					Server::getError_page() const { return this->_error_page; }
size_t						Server::getMaxfilesize() const { return this->_maxfilesize; }
std::vector<Location*>		Server::getLocations() const { return this->_locations; }

std::ostream&		operator<<(std::ostream& o, const std::vector<Server*>& x)
{
	for (size_t i = 0; i < x.size(); ++i) {
		o	<<	"Server name: " 	<< x[i]->getServer_name()	<< std::endl
			<<	"Port: " 			<< x[i]->getPort()			<< std::endl
			<<	"Host: " 			<< x[i]->getHost()			<< std::endl
			<<	"Max filesize: "	<< x[i]->getMaxfilesize()	<< std::endl
			<<	"Error page: "		<< x[i]->getError_page()
									<< x[i]->getLocations();
	}
	return o;
}

std::ostream&		operator<<(std::ostream& o, const std::vector<Location*>& x)
{
	for (size_t i = 0; i < x.size(); ++i)
		o << *x[i];
	return o;
}
