//
// Created by jelle on 3/4/2021.
//

#include "server/parsers/TerminalParser.hpp"

using namespace NotApache;

TerminalParser::TerminalParser(): AParser(TERMINAL, "COM") {}

AParser::formatState	TerminalParser::formatCheck(Client &client) {
	std::string::size_type	i = client.getRawRequest().find('\n');
	if (i == std::string::npos)
		return AParser::UNFINISHED;
	return AParser::FINISHED;
}
