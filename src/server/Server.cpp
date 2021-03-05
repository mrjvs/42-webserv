//
// Created by jelle on 3/2/2021.
//
#include <iostream>



#include "server/Server.hpp"
#include <unistd.h>

using namespace NotApache;

// TODO fix leaks
Server::Server(): _handlerBalance(0), _readFDSet(), _writeFDSet() {}

FD	Server::_maxFD() {
	FD	max = -1;
	for (std::vector<AListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i) {
		FD	currentFD = (*i)->getFD();
		if (currentFD > max) max = currentFD;
	}
	for (std::vector<Client*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		FD	currentFD = (*i)->getReadFD();
		if (currentFD > max) max = currentFD;
	}
	return max;
}

void Server::serve() {
	// start listeners
	for (std::vector<AListener*>::iterator first = _listeners.begin(); first != _listeners.end(); ++first)
		(*first)->start();
	logItem(log::INFO, "Server successfully listening");

	while (true) {
		// make sets
		_createFDSets();

		// wait for FD events
		// TODO timeout
		if (select(_maxFD() + 1, &_readFDSet, &_writeFDSet, NULL, NULL) == -1) {
			// TODO error in select
			logItem(log::ERROR, "Failed to listen to connections");
			return;
		}

		// accept new connections
		for (std::vector<AListener*>::iterator listener = _listeners.begin(); listener != _listeners.end(); ++listener) {
			if (FD_ISSET((*listener)->getFD(), &_readFDSet)) {
				Client *newClient = (*listener)->acceptClient();
				_clients.push_back(newClient);
			}
		}

		for (std::vector<Client*>::iterator c = _clients.begin(); c != _clients.end(); ++c) {
			if (FD_ISSET((*c)->getReadFD(), &_readFDSet) && (*c)->getState() == READING) {
				// handle read
				if (_handlerBalance + 1 >= _handlers.size()) _handlerBalance = 0;
				else _handlerBalance++;
				(_handlers[_handlerBalance])->read(**c);
			}
			else if (FD_ISSET((*c)->getWriteFD(), &_writeFDSet) && (*c)->getState() == WRITING) {
				// handle write
				if (_handlerBalance + 1 >= _handlers.size()) _handlerBalance = 0;
				else _handlerBalance++;
				(_handlers[_handlerBalance])->write(**c);
			}
		}
	}
}

void Server::addListener(AListener *listener) {
	listener->setLogger(*_logger);
	_listeners.push_back(listener);
}

void Server::addHandler(AHandler *handler) {
	handler->setLogger(*_logger);
	handler->setParsers(&_parsers);
	handler->setResponders(&_responders);
	_handlers.push_back(handler);
}

void Server::addParser(AParser *parser) {
	_parsers.push_back(parser);
}

void Server::addResponder(AResponder *responder) {
	_responders.push_back(responder);
}

void Server::_createFDSets() {
	FD_ZERO(&_readFDSet);
	FD_ZERO(&_writeFDSet);
	for (std::vector<AListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i)
		FD_SET((*i)->getFD(), &_readFDSet);
	for (std::vector<Client*>::iterator i = _clients.begin(); i != _clients.end(); ++i) {
		if ((*i)->getState() == READING) FD_SET((*i)->getReadFD(), &_readFDSet);
		else if ((*i)->getState() == WRITING) FD_SET((*i)->getWriteFD(), &_writeFDSet);
	}
}
