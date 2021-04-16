//
// Created by jelle on 3/12/2021.
//

#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include "server/ServerTypes.hpp"
#include "server/http/HTTPClient.hpp"
#include <exception>

namespace NotApache {

	class TCPListener {
	private:
		int _port;
		long _host;
		FD _fd;
		int _backlog;
		sockaddr_in svr_addr;

	public:
		TCPListener(int port, long host, int backLog = 128);
		virtual ~TCPListener();

		FD getFD();
		void start();

		HTTPClient	*acceptClient();

		class FailedToListen : public std::exception {
			const char *what() const throw() {
				return "TCPListener: Failed to bind and listen on port";
			}
		};

		class FailedToAccept : public std::exception {
			const char *what() const throw() {
				return "TCPListener: Failed to accept client";
			}
		};
	};

}

#endif //TCPLISTENER_HPP
