//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPClient.hpp"
#include "utils/intToString.hpp"
#include <unistd.h>

using namespace NotApache;

HTTPClient::HTTPClient(FD clientFd, int port, long host, sockaddr_in cli_addr): _fd(clientFd), _port(port), _host(host), _associatedFds(), _cli_addr(cli_addr), writeState(NO_RESPONSE), connectionState(READING), responseState(NONE), isHandled(false), proxy(), cgi(), concurrentFails(0), serverBlock(0), routeBlock(0), replyStatus(0), hasErrored(false) {
	static unsigned int clientIdCounter = 0;
	timeval timeData;
	::gettimeofday(&timeData, 0);
	_createdAt = timeData.tv_sec;
	_timeoutAfter = 60; // timeout in seconds
    clientIdCounter++;
	clientCount = clientIdCounter;
	clock_gettime(CLOCK_REALTIME, &startTimeData);
	timeLastKey = "Create";
}

HTTPClient::~HTTPClient() {
	for (std::vector<associatedFD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); ++it) {
		::close(it->fd);
	}
	_associatedFds.clear();
	delete cgi;
	delete proxy;
	doTimeLog("Destroy");
}

FD HTTPClient::getFd() const {
	return _fd;
}

int HTTPClient::getPort() const {
	return _port;
}

long HTTPClient::getHost() const {
	return _host;
}

sockaddr_in HTTPClient::getCliAddr() const {
	return _cli_addr;
}

void HTTPClient::addAssociatedFd(FD fd, associatedFD::type mode) {
	for (std::vector<associatedFD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); ++it) {
		if (it->fd == fd)
			return;
	}
	associatedFD newFd = { fd, mode };
	_associatedFds.push_back(newFd);
}

void HTTPClient::removeAssociatedFd(FD fd) {
	for (std::vector<associatedFD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); ++it) {
		if (it->fd == fd) {
			::close(it->fd);
			_associatedFds.erase(it);
			return;
		}
	}
}

void HTTPClient::clearAssociatedFd() {
	for (std::vector<associatedFD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); ++it) {
		::close(it->fd);
	}
	_associatedFds.clear();
}

void HTTPClient::setAssociatedFdMode(FD fd, associatedFD::type mode) {
	for (std::vector<associatedFD>::iterator it = _associatedFds.begin(); it != _associatedFds.end(); ++it) {
		if (it->fd == fd) {
			it->mode = mode;
			return;
		}
	}
}

associatedFD HTTPClient::getAssociatedFd(std::vector<associatedFD>::size_type i) const {
	return _associatedFds[i];
}

std::vector<associatedFD>::size_type HTTPClient::getAssociatedFdLength() const {
	return _associatedFds.size();
}

void HTTPClient::timeout(bool useLocks) {
	if (getTimeDiff() < getTimeoutAfter()) {
		return; // not timed out, continue like normal
	}
	if (useLocks)
		isHandled.lock();
	connectionState = CLOSED;
	if (useLocks)
		isHandled.unlock();
}

std::string HTTPClient::getIp() const {
	const char *ip = reinterpret_cast<const char *>(&_cli_addr.sin_addr.s_addr);

	std::string out;
	for (int i = 0; i < 4; ++i) {
		if (!out.empty())
			out += ".";
		out += utils::intToString(ip[i]);
	}
	return out;
}
long int    HTTPClient::getTimeDiff() const {
    timeval timeData;
    ::gettimeofday(&timeData, 0);
	return timeData.tv_sec - _createdAt;
}

long int	HTTPClient::getTimeoutAfter() const {
	return _timeoutAfter;
}

void HTTPClient::setTimeout(int timeout) {
	_timeoutAfter = timeout;
}
