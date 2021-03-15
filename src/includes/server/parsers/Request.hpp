//
// Created by jelle on 3/3/2021.
//

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "server/parsers/ft_utils.hpp"

#include <map>
#include <iostream>

namespace NotApache {

	enum e_method {
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		CONNECT,
		OPTIONS,
		TRACE
	};
	enum e_headers {
		ACCEPT_CHARSET,
		ACCEPT_LANGUAGE,
		ALLOW,
		AUTHORIZATION,
		//CONNECTION,
		CONTENT_LANGUAGE,
		CONTENT_LENGTH,
		CONTENT_LOCATION,
		CONTENT_TYPE,
		DATE,
		HOST,
		LAST_MODIFIED,
		LOCATION,
		REFERER,
		RETRY_AFTER,		
		SERVER,
		TRANSFER_ENCODING,
		USER_AGENT,
		WWW_AUTHENTICATE
	};

	class Request
	{
		private:

		public:
			enum e_ret {
				OK,
				ERROR
			};

			e_method							_method;
			std::string							_uri;
			std::pair<int, int>					_version;
			std::map<e_headers, std::string>	_headers;
			std::string							_body;
			size_t								_bodySize;
			std::map<std::string, e_method>		_methodMap;
			std::map<std::string, e_headers>	_headerMap;

			//Server*								server;
			//Location*							location;
			//int									status_code;
			//std::string							cgiparams;
			//bool								transfer_buffer,
			//									cgi_ran;

			Request();

			e_ret						parseRequest(std::string request);
			e_ret						parseHeaders(std::string line);
			e_ret						parseRequestLine(std::string reqLine);
			e_ret						parseBody(std::string line);
	};

	std::ostream& operator<<(std::ostream& o, Request& x);
}

#endif //REQUEST_HPP
