#include "includes/Server.hpp"
#include "includes/getLine.hpp"
#include "includes/ft_utils.hpp"
#include "includes/Location.hpp"

#include <iostream>

Server::Server() {}

Server::Server(int fd) {
	(void)fd;
}

Server::~Server() {}

Server::Server(const Server& rhs)
{
	(void)rhs;
}

Server& 	Server::operator=(const Server& rhs)
{
	(void)rhs;
	return *this;
}

void		Server::getKeyAndValue(int fd, std::string& str, std::string& key, std::string& value)
{
	str = str.substr(1);
	key = str.substr(0, str.find_first_of(" \n\t"));
	if (key == "location")
	{
		value = str.substr();
		while (ft::getLine(fd, str) && str != "\t}")
			value += str;
	}
	else
		value = str.substr(str.find_last_of('\t')+1, str.length());
}

void		Server::parse(int fd)
{
	std::string str, key, value;
	std::map<std::string, void(Server::*)(const std::string&)> set;

	set["port"] = &Server::setPort;
	set["host"] = &Server::setHost;
	set["server_name"] = &Server::setServer_name;
	set["error_page"] = &Server::setError_page;
	set["max_filesize"] = &Server::setMaxfilesize;
	set["location"] = &Server::setLocation;
	
	while (ft::getLine(fd, str))
	{
		if (str.empty())
			continue ;
		if (str == "}")
			break ;
		else if (str[0] != '\t')
			throw std::runtime_error("invalid");
		getKeyAndValue(fd, str, key, value);
		try {
			(this->*(set.at(key)))(value);
		}
		catch (const std::exception& e) {
			std::runtime_error("* unknown key in config file *");
			// std::cout << "* unknown key in config file *\n";
		}
	}
	if (!_port || !_maxfilesize || _host.empty() || _server_name.empty() || _error_page.empty())
		throw std::runtime_error("invalid");
}

void	Server::setLocation(const std::string& str)
{
	Location* loc = new Location;
	std::vector<std::string> locSettings = ft::split(str, "\t\n");

	for (size_t i = 0; i < locSettings.size(); ++i) {
		if (locSettings[i].find("location ", 0, 9) != std::string::npos)
			loc->setPrefix(locSettings[i].substr(9));
		else if (locSettings[i].find("root ", 0, 5) != std::string::npos)
			loc->setRoot(locSettings[i].substr(5));
		else if (locSettings[i].find("allow_method ", 0, 13) != std::string::npos)
			loc->setAllow_method(locSettings[i].substr(13));
		else if (locSettings[i].find("autoindex ", 0, 10) != std::string::npos)
			loc->setAutoindex(locSettings[i].substr(10));
		else if (locSettings[i].find("index ", 0, 6) != std::string::npos)
			loc->setIndex(locSettings[i].substr(6));
		else if (locSettings[i].find("maxBody ", 0, 8) != std::string::npos)
			loc->setMaxBody(locSettings[i].substr(8));
	}
	this->_locations.push_back(loc);
}

void	Server::setPort(const std::string& str)
{
	this->_port = ft::stoi(str);
}

void	Server::setHost(const std::string& str)
{
	this->_host = str;
}

void	Server::setServer_name(const std::string& str)
{
	this->_server_name = str;
}

void	Server::setError_page(const std::string& str)
{
	this->_error_page = str;
}

void	Server::setMaxfilesize(const std::string& str)
{
	this->_maxfilesize = ft::stoi(str);
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
