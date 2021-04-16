//
// Created by jelle on 4/12/2021.
//

#include "server/http/HTTPResponder.hpp"
#include "server/http/HTTPMimeTypes.hpp"
#include "utils/base64.hpp"
#include <fcntl.h>
#include <cerrno>
#include <dirent.h>

using namespace NotApache;

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

void HTTPResponder::prepareFile(HTTPClient &client, const struct ::stat &buf, int code) {
	client.data.response.builder.setModifiedDate(buf.STAT_TIME_FIELD);
	prepareFile(client, code);
}

void HTTPResponder::prepareFile(HTTPClient &client, int code) {
	FD fileFd = ::open(client.file.path.c_str(), O_RDONLY);
	if (fileFd == -1) {
		handleError(client, 500);
		return;
	}

	client.data.response.builder.setHeader("Content-Type", MimeTypes::getMimeType(client.file.getExt()));
	client.data.response.builder.setStatus(code);
	client.replyStatus = code;

	// add OPTIONS specific header
	if (client.data.request.data.method == OPTIONS) {
		client.data.response.builder.setAllowedMethods(client.routeBlock->getAllowedMethods());
	}

	// send request for methods that dont send a file
	if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
		client.data.response.builder.removeHeader("CONTENT-LENGTH");
		client.data.response.setResponse(client.data.response.builder.build());
		return;
	}

	// send file
	// TODO TEMP CODE, REMOVE
	if (client.data.request.data.uri.path == "/") {
		client.data.response.setResponse(
			client.data.response.builder
				.setBody("<h1>Hello world</h1>")
				.build()
		);
		return;
	}
	client.addAssociatedFd(fileFd);
	client.responseState = NotApache::FILE;
	client.connectionState = ASSOCIATED_FD;
}

void HTTPResponder::serveFile(HTTPClient &client) {
	struct ::stat buf = {};

	// set accepted language with highest quality rating (=> start at the end of map)
	if (!client.routeBlock->getAcceptLanguage().empty()) {
		std::map<std::string, std::string>::reverse_iterator requestLanguages = client.data.request.data.languageMap.rbegin();
		std::map<std::string, std::string>::reverse_iterator notFound = client.data.request.data.languageMap.rend();

		for (; requestLanguages != notFound; ++requestLanguages) {
			std::vector<std::string>::const_iterator acceptedLanguages = client.routeBlock->getAcceptLanguage().begin();
			for (; acceptedLanguages != client.routeBlock->getAcceptLanguage().end(); ++acceptedLanguages) {
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
			handleError(client, 406); // not acceptable
			return ;
		}
	}
	// if "*" set to first language in config
	if (client.data.request.data.acceptLanguage == "*")
		client.data.request.data.acceptLanguage = client.routeBlock->getAcceptLanguage()[0];

	// get file data
	bool shouldCgi = client.routeBlock->shouldDoCgi() && !client.routeBlock->getCgiExt().empty() && client.file.getExt() == client.routeBlock->getCgiExt();
	if (!shouldCgi || !client.routeBlock->shouldCgiHandleFile()) {
		if (::stat(client.file.path.c_str(), &buf) == -1) {
			if (errno == ENOENT || errno == ENOTDIR)
				handleError(client, 404);
			else
				handleError(client, 500);
			return;
		}

		// check for directory
		if (S_ISDIR(buf.st_mode)) {
			serveDirectory(client, buf);
			return;
		}
		else if (!S_ISREG(buf.st_mode)) {
			handleError(client, 403);
			return;
		}
	}

	// serve the file
	if (shouldCgi) {
		globalLogger.logItem(logger::DEBUG, "Handling cgi request");
		// handle cgi
		try {
			runCGI(client, client.routeBlock->getCgi());
		} catch (std::exception &e) {
			globalLogger.logItem(logger::ERROR, std::string("CGI error: ") + e.what());
			handleError(client, 500);
		}
		return;
	}
	prepareFile(client, buf, 200);
}

void HTTPResponder::serveDirectory(HTTPClient &client, const struct ::stat &directoryStat) {
	// check index
	if (!client.routeBlock->getIndex().empty()) {
		struct ::stat indexData = {};
		utils::Uri indexFile = client.file;
		indexFile.appendPath(client.routeBlock->getIndex());
		if (::stat(indexFile.path.c_str(), &indexData) == -1) {
			if (errno != ENOENT && errno != ENOTDIR) {
				handleError(client, 500);
				return;
			}
		}

		// index file exists, serve it
		if (S_ISREG(indexData.st_mode)) {
			client.file = indexFile;
			utils::Uri rewrittenUrl = client.rewrittenUrl;
			rewrittenUrl.appendPath(client.routeBlock->getIndex());
			client.rewrittenUrl = rewrittenUrl.path;
			if (client.routeBlock->shouldDoCgi() && !client.routeBlock->getCgiExt().empty() && client.file.getExt() == client.routeBlock->getCgiExt()) {
				globalLogger.logItem(logger::DEBUG, "Handling cgi request");
				try {
					runCGI(client, client.routeBlock->getCgi());
				} catch (std::exception &e) {
					globalLogger.logItem(logger::ERROR, std::string("CGI error: ") + e.what());
					handleError(client, 500);
				}
				return;
			}
			prepareFile(client, indexData, 200);
			return;
		}
	}

	// not index, handle directory listing
	if (client.routeBlock->isDirectoryListing()) {
		DIR *dir = ::opendir(client.file.path.c_str());
		if (dir == 0) {
			handleError(client, 500);
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
			builder.setAllowedMethods(client.routeBlock->getAllowedMethods());
		}

		// send request for methods that dont send a body
		if (client.data.request.data.method == HEAD || client.data.request.data.method == OPTIONS) {
			builder.removeHeader("CONTENT-LENGTH");
			client.data.response.setResponse(builder.build());
			return;
		}

		builder.setBody(str);
		client.replyStatus = 200;
		client.data.response.setResponse(
				builder.build()
		);
		return;
	}

	handleError(client, 404);
}
