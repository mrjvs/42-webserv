//
// Created by jelle on 3/4/2021.
//

#ifndef TERMINALPARSER_HPP
#define TERMINALPARSER_HPP

#include "server/parsers/AParser.hpp"

namespace NotApache {

	class TerminalParser: public AParser {
	public:
		TerminalParser();

		AParser::formatState	formatCheck(Client &client);
	};
}

#endif //TERMINALPARSER_HPP
