//
// Created by jelle on 3/4/2021.
//

#include "server/responders/HTTPResponder.hpp"

using namespace NotApache;

HTTPResponder::HTTPResponder(): AResponder("HTTP") {}

std::string HTTPResponder::generateResponse(Client &client) const {
			
	std::cout << client.getRequest() << std::endl;

	return "HTTP response here\n";
}

std::string HTTPResponder::generateParseError(Client &client) const {
	(void)client;
	return "HTTP bad request (parse error)\n";
}

std::string HTTPResponder::generateTimeout(Client &client) const {
	(void)client;
	return "HTTP: connection timed out\n";
}
