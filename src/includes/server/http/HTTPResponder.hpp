//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include <config/blocks/ServerBlock.hpp>
#include "server/http/HTTPClient.hpp"
#include "env/env.hpp"
#include "utils/base64.hpp"

namespace NotApache {
	class HTTPResponder {
	public:
		static void		generateResponse(HTTPClient &client);

		static void generateAssociatedResponse(HTTPClient &client);

		static void serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file);

		static void handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage = true);
		static void handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage = true);

		static void
		serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route,
					   const std::string &dirPath);

		static void setEnv(HTTPClient& client, CGIenv::env& envp, std::string& uri, const std::string& f);
		static void runCGI(HTTPClient& client, const std::string &f, const std::string& cgi);
		static bool checkCredentials(const std::string& authFile, const std::string& credentials);

		class NotFound : public std::exception {
			public:
				virtual const char *what() const throw() { return "CGI not found"; }
		};
		class PipeFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Pipe fail"; }
		};
		class ReadFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Read fail"; }
		};
		class OpenFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Open fail"; }
		};
		class CloseFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Close fail"; }
		};
		class ForkFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Fork fail"; }
		};
		class CWDFail: public std::exception {
			public:
				virtual const char *what() const throw() { return "CWD fail"; }
		};
	};
}

#endif //HTTPRESPONDER_HPP
