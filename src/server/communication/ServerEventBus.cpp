//
// Created by jelle on 3/13/2021.
//

#include "server/communication/ServerEventBus.hpp"

using namespace NotApache;

ServerEventBus::ServerEventBus() {
	int	pipeFds[2];
	if (::pipe(pipeFds) == -1)
		throw PipeCreationFailedException();
	_readEnd = pipeFds[0];
	_writeEnd = pipeFds[1];
}

// TODO make thread safe
ServerEventBus::ServerEventBus(const ServerEventBus &cpy): _writeEnd(cpy._writeEnd), _readEnd(cpy._readEnd) {
	_queuedEvents[CLIENT_STATE_UPDATED] = false;
	_queuedEvents[START_QUEUE] = false;
}

FD	ServerEventBus::getReadFD() {
	return _readEnd;
}

ServerEventBus::Events ServerEventBus::getPostedEvent() {
	char c;
	if (::read(_readEnd, &c, 1) == -1)
		throw FailedToGetEvent();
	Events event = static_cast<Events>(c);
	_queuedEvents[event] = false;
	return static_cast<Events>(c);
}

void	ServerEventBus::postEvent(ServerEventBus::Events event) {
	// if already a queued event, dont add
	if (_queuedEvents[event]) return;

	char c = static_cast<char>(event);
	if (::write(_writeEnd, &c, 1) == -1)
		throw FailedToPostEvent();
	_queuedEvents[event] = true;
}

ServerEventBus::~ServerEventBus() {
	::close(_readEnd);
	::close(_writeEnd);
}
