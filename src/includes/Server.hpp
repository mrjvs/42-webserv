#pragma once

#include <string>
#include <map>
#include "Location.hpp"

class Server
{
	public:
		Server();
		explicit Server(int fd);

		void			parse(int fd);

		void			getKeyAndValues(int fd, std::string& line, std::string& key, std::vector<std::string>& values);

		void			setPort(const std::vector<std::string>& args);
		void			setHost(const std::vector<std::string>& args);
		void			setServer_name(const std::vector<std::string>& args);
		void			setError_page(const std::vector<std::string>& args);
		void			setMaxfilesize(const std::vector<std::string>& args);
		void			setLocation(const std::vector<std::string>& args);

		size_t						getPort() const;
		std::string					getHost() const;
		std::string					getServer_name() const;
		std::string					getError_page() const;
		size_t						getMaxfilesize() const;
		std::vector<Location*>		getLocations() const;


	private:
		size_t						_port;
		long int					_maxfilesize;
		std::string 				_host,
									_server_name,
									_error_page;
		std::vector<Location*> 		_locations;
		// 							_root,
		// 							_autoindex;
		// int							_fd,
		// 							_socketFd;
		// std::vector<std::string> 	_indexes;
		// struct sockaddr_in			addr;
};

std::ostream&		operator<<(std::ostream& o, const std::vector<Server*>& serv);

std::ostream&		operator<<(std::ostream& o, const std::vector<Location*>& serv);

