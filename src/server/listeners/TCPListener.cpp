//
// Created by jelle on 3/2/2021.
//

#include "server/listeners/TCPListener.hpp"
#include <unistd.h>
#include <fcntl.h>

using namespace NotApache;

TCPListener::TCPListener(int port, int backLog): _port(port), _fd(-1), _backlog(backLog) {}

FD TCPListener::getFD() {
	return _fd;
}

void TCPListener::start() {
	int one = 1;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		logItem(logger::ERROR, "Socket opening failed!");
		throw FailedToListen();
	}

	// set socket options
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons(_port);

	if (bind(_fd, reinterpret_cast<struct sockaddr *>(&svr_addr), sizeof(svr_addr)) == -1) {
		close(_fd);
		_fd = -1;
		logItem(logger::ERROR, "Failed to bind socket to local network!");
		throw FailedToListen();
	}

	if (::listen(_fd, _backlog) == -1) {
		close(_fd);
		_fd = -1;
		logItem(logger::ERROR, "Failed to listen on port!");
		throw FailedToListen();
	}
	logItem(logger::INFO, "Server is listening!");
}

TCPListener::~TCPListener() {
	if (_fd > 0) close(_fd);
}

Client	*TCPListener::acceptClient() {
	sockaddr_in	cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int client_fd = accept(_fd, (struct sockaddr *) &cli_addr, &sin_len);
	logItem(logger::DEBUG, "Client connected");

	if (client_fd == -1) {
		logItem(logger::ERROR, "Client not accepted");
		throw FailedToAccept();
	}

	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	Client	*out = new Client(client_fd, client_fd);
	out->setTimeout(30);
	return out;
}
