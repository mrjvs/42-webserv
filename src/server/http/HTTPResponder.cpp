//
// Created by jelle on 3/13/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "utils/intToString.hpp"
#include "utils/strdup.hpp"
#include "utils/ErrorThrow.hpp"
#include "server/http/ResponseBuilder.hpp"
#include "server/http/HTTPParser.hpp"
#include "env/ENVBuilder.hpp"
#include "server/global/GlobalLogger.hpp"
#include "server/global/GlobalConfig.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>
#include <unistd.h>
#include "server/http/HTTPMimeTypes.hpp"

using namespace NotApache;

// TODO cleanup this file

void HTTPResponder::generateAssociatedResponse(HTTPClient &client) {
	config::ServerBlock *server = configuration->findServerBlock(client.data.request.data.headers.find("HOST")->second, client.getPort(), client.getHost());
	if (client.responseState == FILE) {
		client.data.response.setResponse(
			client.data.response.builder
			.setBody(client.data.response.getAssociatedDataRaw())
			.build()
		);
	} else if (client.responseState == UPLOAD) {
		client.data.response.setResponse(
			client.data.response.builder.build()
		);
	} else if (client.responseState == PROXY) {
		if (client.proxy->response.data.parseStatusCode != 200) {
			handleError(client, server, 502, false);
			return;
		}
		client.data.response.setResponse(
			ResponseBuilder(client.proxy->response.data)
			.build()
		);
	}
	else if (client.responseState == CGI) {
		// parse error
		if (client.cgi->response.data.parseStatusCode != 200) {
			handleError(client, server, 500, false);
			return;
		}

		// child process exit codes
		if (client.cgi->status != 0) {
			if (client.cgi->status == EXECVE_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: execve");
			else if (client.cgi->status == CLOSE_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: close");
			else if (client.cgi->status == DUP2_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: dup2");
			else if (client.cgi->status == CHDIR_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: chdir");
			else if (client.cgi->status == GETCWD_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: getcdw");
			else if (client.cgi->status == MEMORY_ERROR)
				globalLogger.logItem(logger::ERROR, "CGI error: memory");
			handleError(client, server, 500, false);
			return;
		}

		// cgi success, respond normally
		client.data.response.setResponse(
			ResponseBuilder(client.cgi->response.data)
			.build()
		);
	}
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage) {
	handleError(client, server, 0, code, doErrorPage);
}

void HTTPResponder::handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage) {
	// Request authentication
	if (code == 401 && route)
		client.data.response.builder.setHeader("WWW-AUTHENTICATE", "Basic realm=\"Not-Apache\"");

	// allow header in 405
	if (code == 405 && route) {
		client.data.response.builder.setAllowedMethods(route->getAllowedMethods());
	}

	// handle error pages
	if (doErrorPage && server != 0 && !server->getErrorPage(code).empty()) {
		struct ::stat errorPageData = {};
		utils::Uri errorPageFile(server->getErrorPage(code));
		if (::stat(errorPageFile.path.c_str(), &errorPageData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, server, route, 500, false);
				return;
			}
		}
		if (S_ISREG(errorPageData.st_mode)) {
			prepareFile(client, *server, *route, errorPageFile, code, false);
			return;
		}
	}

	// generate error page
	client.data.response.builder
		.setStatus(code)
		.setHeader("Content-Type", "text/html");

	std::map<int,std::string>::const_iterator statusIt = ResponseBuilder::statusMap.find(code);
	std::string text = statusIt == ResponseBuilder::statusMap.end() ? "Internal server error!" : statusIt->second;
	client.data.response.builder.setBody(std::string("<h1>") + utils::intToString(code) + "</h1><p>" + text + "</p>");
	client.data.response.setResponse(client.data.response.builder.build());
}

void HTTPResponder::serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct stat &directoryStat, const std::string &d) {
	// check index
	utils::Uri dirPath = d;
	if (!route.getIndex().empty()) {
		struct ::stat indexData = {};
		utils::Uri indexFile = d;
		indexFile.appendPath(route.getIndex());
		if (::stat(indexFile.path.c_str(), &indexData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, &server, 500);
				return;
			}
		}

		// index file exists, serve it
		if (S_ISREG(indexData.st_mode)) {
			// TODO index files need cgi too
			prepareFile(client, server, route, indexData, indexFile);
			return;
		}
	}

	// not index, handle directory listing
	if (route.isDirectoryListing()) {
		DIR *dir = ::opendir(dirPath.path.c_str());
		if (dir == 0) {
			handleError(client, &server, 500);
			return;
		}
		dirent *dirEntry;
		utils::Uri uri = client.data.request.data.uri;
		std::string str = "<h1>";
		str += uri.path + "</h1><ul>";
		while ((dirEntry = ::readdir(dir)) != 0) {
			utils::Uri path = uri;
			path.appendPath(dirEntry->d_name);
			str += "<li><a href=\"";
			str += path.path;
			str += "\">";
			if (dirEntry->d_type == DT_DIR) {
				str += "DIR ";
			}
			str += dirEntry->d_name;
			str += "</a></li>";
		}
		str += "</ul>";
		::closedir(dir);

		ResponseBuilder builder;
		builder
		.setModifiedDate(directoryStat.STAT_TIME_FIELD)
		.setHeader("Content-Type", "text/html");

		// add OPTIONS specific header
		if (client.data.request.data.method == OPTIONS) {
			builder.setAllowedMethods(route.getAllowedMethods());
		}

		// send request for methods that dont send a body
		if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
			builder.removeHeader("CONTENT-LENGTH");
			client.data.response.setResponse(builder.build());
			return;
		}

		builder.setBody(str);
		client.data.response.setResponse(
			builder.build()
		);
		return;
	}

	// normal directory handler (also cmon, it should be 403, not 404. stupid 42)
	handleError(client, &server, 404);
}

void HTTPResponder::prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const utils::Uri &file, int code, bool shouldErrorFile) {
	FD fileFd = ::open(file.path.c_str(), O_RDONLY);
	if (fileFd == -1) {
		handleError(client, &server, 500, shouldErrorFile);
		return;
	}

	client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(file.getExt()));
	client.data.response.builder.setStatus(code);

	// add OPTIONS specific header
	// TODO options for * uri
	if (client.data.request.data.method == OPTIONS) {
		client.data.response.builder.setAllowedMethods(route.getAllowedMethods());
	}

	// send request for methods that dont send a file
	if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
		client.data.response.builder.removeHeader("CONTENT-LENGTH");
		client.data.response.setResponse(client.data.response.builder.build());
		return;
	}

	// send file
	client.addAssociatedFd(fileFd);
	client.responseState = NotApache::FILE;
	client.connectionState = ASSOCIATED_FD;
}

void HTTPResponder::prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &buf, const utils::Uri &file, int code) {
	client.data.response.builder.setModifiedDate(buf.STAT_TIME_FIELD);
	prepareFile(client, server, route, file, code);
}

void HTTPResponder::serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f, const std::string &rewrittenUrl) {
	struct ::stat buf = {};

	// check autorization
	if (!route.getAuthBasic().empty()) {
		std::map<std::string, std::string>::iterator it = client.data.request.data.headers.find("AUTHORIZATION");
		if (it == client.data.request.data.headers.end()) {
			handleError(client, &server, &route, 401);
			return ;
		}
		else {
			try {
				if (!checkCredentials(route.getAuthorized(), it->second)) {
					handleError(client, &server, &route, 403);
					return ;
				}
			}
			catch (const std::exception& e) {
				globalLogger.logItem(logger::ERROR, std::string("File serving error: ") + e.what());
				handleError(client, &server, &route, 500);
			}
		}
	}

	// set accepted language with highest quality rating (=> start at the end of map)
	if (!route.getAcceptLanguage().empty()) {
		std::map<std::string, std::string>::reverse_iterator requestLanguages = client.data.request.data.languageMap.rbegin();
		std::map<std::string, std::string>::reverse_iterator notFound = client.data.request.data.languageMap.rend();

		for (; requestLanguages != notFound; ++requestLanguages) {
			std::vector<std::string>::const_iterator acceptedLanguages = route.getAcceptLanguage().begin();
			for (; acceptedLanguages != route.getAcceptLanguage().end(); ++acceptedLanguages) {
				if (requestLanguages->second == *acceptedLanguages) {
					client.data.request.data.acceptLanguage = requestLanguages->second;
					break ;
				}
			}
			if (!client.data.request.data.acceptLanguage.empty())
				break ;
		}
		// error if no match was found
		if (client.data.request.data.acceptLanguage.empty()) {
			handleError(client, &server, &route, 406); // not acceptable
			return ;
		}
	}
	// if "*" set to first language in config
	if (client.data.request.data.acceptLanguage == "*")
		client.data.request.data.acceptLanguage = route.getAcceptLanguage()[0];

	// get file data
	// TODO cgi needs magic path parsing -> https://tools.ietf.org/html/rfc3875#section-3.2
	utils::Uri file = f;
	bool shouldCgi = route.shouldDoCgi() && !route.getCgiExt().empty() && file.getExt() == route.getCgiExt();
	if (!shouldCgi || !route.shouldCgiHandleFile()) {
		if (::stat(file.path.c_str(), &buf) == -1) {
			if (errno == ENOENT || errno == ENOTDIR)
				handleError(client, &server, &route, 404);
			else
				handleError(client, &server, &route, 500);
			return;
		}

		// check for directory
		if (S_ISDIR(buf.st_mode)) {
			serveDirectory(client, server, route, buf, file.path);
			return;
		}
		else if (!S_ISREG(buf.st_mode)) {
			handleError(client, &server, &route, 403);
			return;
		}
	}

	// serve the file
	if (shouldCgi) {
		globalLogger.logItem(logger::DEBUG, "Handling cgi request");
		// handle cgi
		try {
			runCGI(client, route, route.getCgi(), rewrittenUrl);
		} catch (std::exception &e) {
			globalLogger.logItem(logger::ERROR, std::string("CGI error: ") + e.what());
			handleError(client, &server, &route, 500);
		}
		return;
	}
	prepareFile(client, server, route, buf, file);
}

bool HTTPResponder::checkCredentials(const std::vector<std::string>& authorizedUsers, const std::string& requestUser) {
	// Check header
	if (requestUser.find("Basic ", 0, 6) != 0)
		ERROR_THROW(AuthHeader());

	// Find match
	for (size_t i = 0; i < authorizedUsers.size(); ++i) {
		if (utils::base64_decode(requestUser.substr(6)) == authorizedUsers[i])
			return true;
	}
	return false;
}

void HTTPResponder::uploadFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct ::stat buf = {};
	std::string message = "Successfully updated file!";

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno != ENOENT) {
			if (errno == ENOTDIR)
				handleError(client, &server, &route, 404);
			else
				handleError(client, &server, &route, 500);
			return;
		}
		message = "Successfully created file!";
	}
	else {
		// 403 on anything that isnt a regular file
		if (!S_ISREG(buf.st_mode)) {
			handleError(client, &server, 403);
			return;
		}
	}

	// create the file
	FD uploadFd = ::open(file.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (uploadFd == -1) {
		if (errno == ENOENT)
			handleError(client, &server, 404);
		else
			handleError(client, &server, 500);
		return;
	}

	// setup client for uploading
	client.data.response.builder.setHeader("Content-Type", "text/html");
	client.data.response.builder.setBody(std::string("<h1>") + message + "</h1>");
	client.addAssociatedFd(uploadFd, associatedFD::WRITE);
	client.responseState = NotApache::UPLOAD;
	client.connectionState = ASSOCIATED_FD;
}

void HTTPResponder::deleteFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &f) {
	struct ::stat buf = {};

	// get file data
	utils::Uri file = f;
	if (::stat(file.path.c_str(), &buf) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			handleError(client, &server, &route, 404);
		else
			handleError(client, &server, &route, 500);
		return;
	}

	// only allow removing of normal files
	if (!S_ISREG(buf.st_mode)) {
		handleError(client, &server, &route, 403);
		return;
	}

	// remove the file
	int result = ::unlink(file.path.c_str());
	if (result == -1) {
		handleError(client, &server, &route, 500);
		return;
	}

	client.data.response.setResponse(
		ResponseBuilder()
		.setHeader("Content-Type", "text/html")
		.setBody("<h1>Successfully deleted file!</h1>")
		.build()
	);
}

void HTTPResponder::generateResponse(HTTPClient &client) {
	if (client.data.request.data.parseStatusCode != 200) {
		// error responses if parsing failed
		handleError(client, 0, client.data.request.data.parseStatusCode);
		return;
	}
	std::map<std::string,std::string>::iterator hostIt = client.data.request.data.headers.find("HOST");
	std::string	domain = (*hostIt).second;
	domain = domain.substr(0, domain.find(':'));

	// what server do you belong to?
	config::ServerBlock	*server = configuration->findServerBlock(domain, client.getPort(), client.getHost());
	if (server == 0) {
		handleError(client, server, 400);
		return;
	}

	// find which route block to use
	utils::Uri uri = client.data.request.data.uri;
	std::string rewrittenUrl = uri.path; // findRoute will rewrite url
	config::RouteBlock	*route = server->findRoute(rewrittenUrl);
	if (route == 0) {
		handleError(client, server, 400);
		return;
	}

	// check allowed methods
	if (!route->isAllowedMethod(HTTPParser::methodMap_EtoS.find(client.data.request.data.method)->second)) {
		handleError(client, server, route, 405);
		return;
	}

	if (route->shouldDoFile()) {
		utils::Uri file = route->getRoot();

		// use upload directory instead for upload modifications
		if (client.data.request.data.method == DELETE || client.data.request.data.method == PUT)
			file = utils::Uri(route->getSaveUploads());
		file.appendPath(rewrittenUrl);
		if (client.data.request.data.method == DELETE)
			deleteFile(client, *server, *route, file.path);
		else if (client.data.request.data.method == PUT)
			uploadFile(client, *server, *route, file.path);
		else
			serveFile(client, *server, *route, file.path, rewrittenUrl);
		return;
	}
	else {
		handleProxy(client, server, route);
		return;
	}
}

void HTTPResponder::handleProxy(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route) {
	globalLogger.logItem(logger::DEBUG, "Handling the proxy connection");

	try {

		client.proxy = new Proxy(route->getProxyUrl().ip, route->getProxyUrl().port);
		client.proxy->createConnection();

		client.addAssociatedFd(client.proxy->getSocket(), associatedFD::WRITE);
		client.responseState = PROXY;
		client.connectionState = ASSOCIATED_FD;

		std::string host = route->getProxyUrl().ip + ":" + utils::intToString(route->getProxyUrl().port);
		std::string x_client = client.getIp();
		std::string x_host = client.data.request.data.headers.find("HOST")->second;
		std::string x_proto = route->getProxyUrl().protocol;

		client.proxy->request.setRequest(
			RequestBuilder(client.data.request.data)
			.setHeader("HOST", host)
			.setHeader("CONNECTION", "Close") // always set so it doesn't hang
			.setHeader("X-FORWARDED-FOR", x_client)
			.setHeader("X-FORWARDED-HOST", x_host)
			.setHeader("X-FORWARDED-PROTO", x_proto)
			.build()
		);

	} catch (Proxy::SocketException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 500);
	} catch (Proxy::ConnectionException &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 502);
	} catch (std::exception &e) {
		globalLogger.logItem(logger::ERROR, std::string("Proxy error: ") + e.what());
		handleError(client, server, route, 500);
	}
}

void	HTTPResponder::closePipes(FD *pipefd0, FD *pipefd1, FD *bodyPipefd0, FD *bodyPipefd1, bool closeFail) {	
	if (pipefd0) {
		if (::close(*pipefd0) == -1)
			closeFail = true;
	}
	if (pipefd1) {
		if (::close(*pipefd1) == -1)
			closeFail = true;
	}
	if (bodyPipefd0) {
		if (::close(*bodyPipefd0) == -1)
			closeFail = true;
	}
	if (bodyPipefd1) {
		if (::close(*bodyPipefd1) == -1)
			closeFail = true;
	}
	if (closeFail)
		ERROR_THROW(CgiClass::CloseFail());
}

// TODO fix close issues (if one doesnt get closed, the others dont get closed either which makes it a fd leak)
void	HTTPResponder::runCGI(HTTPClient& client, config::RouteBlock &route, const std::string& cgiPath, const std::string &rewrittenUrl) {
	FD				pipefd[2];
	FD				bodyPipefd[2];
	struct stat 	sb;
	bool 			body = false;
	client.cgi = new CgiClass;

	char curCwd[1024];
	if (::getcwd(curCwd, 1023) == NULL)
		ERROR_THROW(CgiClass::NotFound());
	utils::Uri curCwdUri(curCwd);
	if (cgiPath[0] == '/')
		curCwdUri = utils::Uri(cgiPath);
	else
		curCwdUri.appendPath(cgiPath, true);
	
	client.cgi->generateENV(client, client.data.request.data.uri, rewrittenUrl);
	char** args = new char *[3]();
	args[0] = utils::strdup(curCwdUri.path);
	args[1] = utils::strdup(rewrittenUrl.substr(1));

	if (::stat(args[0], &sb) == -1)
		ERROR_THROW(CgiClass::NotFound());

	if (::pipe(pipefd) == -1)
		ERROR_THROW(CgiClass::PipeFail());
	if (::pipe(bodyPipefd) == -1) {
		closePipes(&pipefd[0], &pipefd[1], NULL, NULL, false);
		ERROR_THROW(CgiClass::PipeFail());
	}

	long int bodyLen = client.data.request.data.isChunked ? client.data.request.data.chunkedData.size() : client.data.request.data.data.size();
	if (bodyLen > 0)
		body = true;

	client.cgi->pid = ::fork();
	client.cgi->hasExited = false;
	if (client.cgi->pid == -1) {
		closePipes(&pipefd[0], &pipefd[1], &bodyPipefd[0], &bodyPipefd[1], false);
		ERROR_THROW(CgiClass::ForkFail());
	}
	if (!client.cgi->pid) {
		char buf[1024];

		if (::chdir(route.getRoot().c_str()) == -1)
			::exit(CHDIR_ERROR);

		if (::getcwd(buf, 1023) == NULL)
			::exit(GETCWD_ERROR);

		try {
			std::string pathTranslated = std::string(buf);
			for (char **envp = client.cgi->getEnvp().getEnv(); *envp != NULL; envp++) {
				std::string envStr = *envp;
				if (envStr.find("DOCUMENT_ROOT=") == 0) {
					envStr += pathTranslated;
					delete [] *envp;
					*envp = utils::strdup(const_cast<char *>(envStr.c_str()));
					break ;
				}
			}
			pathTranslated += rewrittenUrl;
			pathTranslated.insert(0, "PATH_TRANSLATED=");
			for (char **envp = client.cgi->getEnvp().getEnv(); *envp != NULL; envp++) {
				std::string envStr = *envp;
				if (envStr.find("PATH_TRANSLATED=") == 0) {
					delete [] *envp;
					*envp = utils::strdup(const_cast<char *>(pathTranslated.c_str()));
					break ;
				}
			}
		} catch (std::exception &e) {
			::exit(MEMORY_ERROR);
		}
		
		// set body pipes
		if (::dup2(bodyPipefd[0], STDIN_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(bodyPipefd[0]) == -1)
			::exit(CLOSE_ERROR);
		if (::close(bodyPipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// set output pipes
		if (::dup2(pipefd[1], STDOUT_FILENO) == -1)
			::exit(DUP2_ERROR);
		if (::close(pipefd[0]) == -1 || ::close(pipefd[1]) == -1)
			::exit(CLOSE_ERROR);

		// run cgi
		::execve(curCwdUri.path.c_str(), args, client.cgi->getEnvp().getEnv());
		::exit(EXECVE_ERROR);
	}
	delete args[0];
	delete args[1];
	delete [] args;

	// CURRENT PROCESS
	if (::close(pipefd[1]) == -1)
		closePipes(&pipefd[0], NULL, &bodyPipefd[0], &bodyPipefd[1], true);
	if (::close(bodyPipefd[0]) == -1)
		closePipes(&pipefd[0], NULL, NULL, &bodyPipefd[1], true);

	client.addAssociatedFd(pipefd[0]);
	if (body)
		client.addAssociatedFd(bodyPipefd[1], associatedFD::WRITE);
	else if (::close(bodyPipefd[1]) == -1)
		closePipes(&pipefd[0], NULL, NULL, NULL, true);

	client.responseState = CGI;
	client.connectionState = ASSOCIATED_FD;
}
