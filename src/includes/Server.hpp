#pragma once

#include <string>
#include <map>
#include "Location.hpp"

class Server
{
	public:
		Server();
		explicit Server(int fd);
		~Server();
		Server(const Server& rhs);
		Server& 		operator=(const Server& rhs);

		void			parse(int fd);

		void			getKeyAndValue(int fd, std::string& str, std::string& key, std::string& value);

		void			setPort(const std::string& str);
		void			setHost(const std::string& str);
		void			setServer_name(const std::string& str);
		void			setError_page(const std::string& str);
		void			setMaxfilesize(const std::string& str);
		void			setLocation(const std::string& str);

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
};

std::ostream&		operator<<(std::ostream& o, const std::vector<Server*>& serv);

std::ostream&		operator<<(std::ostream& o, const std::vector<Location*>& serv);

// std::vector<std::string> 	_indexes;
// struct sockaddr_in			addr;
