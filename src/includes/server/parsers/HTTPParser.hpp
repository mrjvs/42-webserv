//
// Created by jelle on 3/3/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/parsers/AParser.hpp"
#include <iostream>

namespace NotApache {

	#define HTTP_VERSION "HTTP/1.1"

	enum e_method {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        ERROR
    };

	enum e_ret {
		VALID,
		INVALID
	};

	class HTTPParser: public AParser {
	public:
		HTTPParser();

		AParser::formatState	formatCheck(Client &client) const;
		int						parseRequestLine(std::string& reqLine) const;
		std::map<std::string, e_method>	methodMap;
	};

	template <typename T>
	std::ostream& operator<<(const std::ostream&, const std::vector<T>& x);
}

#endif //HTTPPARSER_HPP
