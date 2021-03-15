#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"

// listeners
#include "server/listeners/TCPListener.hpp"
#include "server/listeners/TerminalListener.hpp"

// handlers
#include "server/handlers/StandardHandler.hpp"

// responders
#include "server/responders/HTTPResponder.hpp"
#include "server/responders/TerminalResponder.hpp"

// parsers
#include "server/parsers/HTTPParser.hpp"
#include "server/parsers/TerminalParser.hpp"

using namespace NotApache;

int main() {
	logger::Logger logger = std::cout;
	logger.setFlags(logger::Flags::Debug | logger::Flags::Color);

	Server server;
	server.setLogger(logger);

	TCPListener *portListen = new TCPListener(8080);
	TerminalListener *termListen = new TerminalListener();
	server.addListener(portListen);
	server.addListener(termListen);

	StandardHandler *handler = new StandardHandler();
	server.addHandler(handler);

	HTTPResponder *httpResponder = new HTTPResponder();
	TerminalResponder *terminalResponder = new TerminalResponder();
	server.addResponder(httpResponder);
	server.addResponder(terminalResponder);

	HTTPParser *httpParser = new HTTPParser();
	httpParser->setLogger(logger);
	TerminalParser *terminalParser = new TerminalParser();
	server.addParser(httpParser);
	server.addParser(terminalParser);

	try {
		server.serve();
	} catch (std::exception &e) {
		logger.log(logger::LogItem(logger::ERROR, e.what()));
	}
	return 0;
}
