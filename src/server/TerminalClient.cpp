//
// Created by jelle on 3/4/2021.
//

#include <iostream>

#include "server/TerminalClient.hpp"
#include <unistd.h>

using namespace NotApache;

TerminalClient::TerminalClient(FD readFD, FD writeFD): Client(readFD, writeFD, TERMINAL) {}

void TerminalClient::close(bool reachedEOF) {
	if (reachedEOF) {
		setState(CLOSED);
		return;
	}
	const std::string &request = getRawRequest();
	setRawRequest(request.substr(request.find('\n')+1));
	setState(READING);
}

// no timeout on terminal commands
void TerminalClient::timeout() {}
