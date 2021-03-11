//
// Created by jelle on 3/3/2021.
//

#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "server/parsers/AParser.hpp"
#include <iostream>

namespace NotApache {

	class HTTPParser : public AParser {
		enum e_ret {
			VALID,
			INVALID,
			CHUNKED,
			BODY
		};
		
		public:
			HTTPParser();

			AParser::formatState	formatCheck(Client &client) const;
			
			int						formatCheckReqLine(const std::string& reqLine) const;
			int						formatCheckHeaders(const std::string& line) const;
			int						formatCheckBody(const std::string& line) const;
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x);

}

#endif //HTTPPARSER_HPP
