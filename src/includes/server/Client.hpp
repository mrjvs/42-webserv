//
// Created by jelle on 3/2/2021.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/time.h>
#include "server/ServerTypes.hpp"
#include "server/parsers/AParser.hpp"
#include "utils/mutex.hpp"

namespace NotApache {
	///	Client connection, holds FD and the current data of the connection
	class Client {
	private:
		FD					_readFD;
		FD					_writeFD;
		ClientTypes			_type;
		ClientStates		_state;
		std::string			_dataType;
		std::string			_request;
		std::string			_response;
		std::size_t			_responseIndex;
		ResponseStates		_responseState;
		timeval				_created;
		unsigned long		_timeoutSeconds;

	public:
		Client(FD readFD, FD writeFD, ClientTypes type = CONNECTION);
		virtual ~Client();

		utils::MutexLock	modifiedLock;
		bool 				isHandled;

		FD				getReadFD() const;
		FD				getWriteFD() const;
		ClientStates	getState() const;
		std::string		getRequest() const;
		ClientTypes		getType() const;
		std::string		getDataType() const;
		std::string		getResponse() const;
		std::size_t		getResponseIndex() const;
		ResponseStates	getResponseState() const;
		const timeval	&getCreatedAt() const;

		void	setWriteFD(FD fd);
		void	setReadFD(FD fd);
		void 	setTimeout(unsigned long seconds);
		void 	setState(ClientStates state);
		void 	setDataType(const std::string &str);
		void 	setResponseState(ResponseStates state);

		void	appendRequest(const std::string &str);
		void	setRequest(const std::string &str);

		void	appendResponse(const std::string &str);
		void	setResponse(const std::string &str);
		void 	setResponseIndex(std::size_t i);

		virtual void	close(bool reachedEOF);
		/// check and handle timeout
		virtual void 	timeout();

		virtual ssize_t	read(char *buf, size_t len);
		virtual ssize_t	write(const char *buf, size_t len);
	};
}

#endif //CLIENT_HPP
