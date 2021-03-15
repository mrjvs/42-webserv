//
// Created by jelle on 3/3/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/parsers/AParser.hpp"
#include "log/Loggable.hpp"
#include <iostream>

namespace NotApache {

	class HTTPParser : public AParser, public logger::ILoggable {
		enum e_ret {
			VALID,
			INVALID,
			CHUNKED,
			BODY
		};
		
		public:
			HTTPParser();

			AParser::formatState	formatCheck(Client &client);
			
			int						formatCheckReqLine(const std::string& reqLine);
			int						formatCheckHeaders(const std::string& line);
			int						formatCheckBody(const std::string& line);
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x);

}

#endif //HTTPPARSER_HPP
