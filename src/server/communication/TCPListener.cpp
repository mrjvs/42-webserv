//
// Created by jelle on 3/12/2021.
//

#include "server/communication/TCPListener.hpp"
#include "server/global/GlobalLogger.hpp"
#include "utils/ErrorThrow.hpp"
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

using namespace NotApache;

TCPListener::TCPListener(int port, long host, int backLog): _port(port), _host(host), _fd(-1), _backlog(backLog) {}

TCPListener::~TCPListener() {
	if (_fd > 0) ::close(_fd);
}

FD TCPListener::getFD() {
	return _fd;
}

void TCPListener::start() {
	_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		globalLogger.logItem(logger::ERROR, "Socket opening failed!");
		ERROR_THROW(FailedToListen());
	}

	// set socket options
	int one = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)); // reuse local addresses on bind
	one = 1;
	setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(int)); // keep the connection alive
	one = 1;
	setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &one, sizeof(int)); // receive urgent tcp data in normal queue (http doesnt work with urgent data)
	one = 1;
	setsockopt(_fd, SOL_SOCKET, SO_LINGER, &one, sizeof(int)); // make sure close will close at appropriate times
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = _host;
	svr_addr.sin_port = htons(_port);

	if (::bind(_fd, reinterpret_cast<struct sockaddr *>(&svr_addr), sizeof(svr_addr)) == -1) {
		::close(_fd);
		_fd = -1;
		globalLogger.logItem(logger::ERROR, "Failed to bind socket to local network!");
		ERROR_THROW(FailedToListen());
	}

	if (::listen(_fd, _backlog) == -1) {
		::close(_fd);
		_fd = -1;
		globalLogger.logItem(logger::ERROR, "Failed to listen on port!");
		ERROR_THROW(FailedToListen());
	}
}

HTTPClient *TCPListener::acceptClient() {
	sockaddr_in	cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int client_fd = accept(_fd, (struct sockaddr *) &cli_addr, &sin_len);
	if (client_fd == -1) {
		globalLogger.logItem(logger::ERROR, "Client not accepted");
		std::cerr << std::strerror(errno) << std::endl;
		ERROR_THROW(FailedToAccept());
	}

	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	HTTPClient	*out = new HTTPClient(client_fd, _port, _host, cli_addr);
	return out;
}
