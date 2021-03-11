//
// Created by jelle on 3/3/2021.
//

#include "server/parsers/Request.hpp"

#include <iostream>

namespace NotApache
{
    std::string methodAsString(const e_method& in) {
        switch (in) {
            case GET:
                return "GET";
            case HEAD:
                return "HEAD";
            case POST:
                return "POST";
            case PUT:
                return "PUT";
			case DELETE:
                return "DELETE";
			case CONNECT:
                return "CONNECT";
			case OPTIONS:
                return "OPTIONS";
			case TRACE:
                return "TRACE";
            default:
                return "INVALID METHOD";
        }
	}

	std::string headerAsString(const e_headers& in) {
        switch (in) {
			case ACCEPT_CHARSET:
				return "ACCEPT-CHARSET";
			case ACCEPT_LANGUAGE:
				return "ACCEPT-LANGUAGE";
			case ALLOW:
				return "ALLOW";
			case AUTHORIZATION:
				return "AUTHORIZATION";
			//case //CONNECTION:
			//	return "CONNECTION";
			case CONTENT_LANGUAGE:
				return "CONTENT-LANGUAGE";
			case CONTENT_LENGTH:
				return "CONTENT-LENGTH";
			case CONTENT_LOCATION:
				return "CONTENT-LOCATION";
			case CONTENT_TYPE:
				return "CONTENT-TYPE";
			case DATE:
				return "DATE";
			case HOST:
				return "HOST";
			case LAST_MODIFIED:
				return "LAST-MODIFIED";
			case LOCATION:
				return "LOCATION";
			case REFERER:
				return "REFERER";
			case RETRY_AFTER:
				return "RETRY-AFTER";	
			case SERVER:
				return "SERVER";
			case TRANSFER_ENCODING:
				return "TRANSFER-ENCODING";
			case USER_AGENT:
				return "USER-AGENT";
			case WWW_AUTHENTICATE:
				return "WWW-AUTHENTICATE";
			default:
				return "INVALID HEADER";
        }
    }

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x) {
		for (size_t i = 0; i < x.size(); ++i)
			o << x[i] << std::endl;
		return o;
	}

	std::ostream& operator<<(std::ostream& o, Request& x) {
		o	<< "==REQUEST=="								<< std::endl
			<< "Method: "	<< methodAsString(x._method)	<< std::endl
			<< "URI: "		<< x._uri						<< std::endl
			<< "VERSION: HTTP/"	<< x._version.first << "." << x._version.second	<< std::endl;
			if (x._headers.size()) {
				o << std::endl << "-HEADERS-" << std::endl;
				for (std::map<e_headers, std::string>::iterator it = x._headers.begin(); it != x._headers.end(); ++it)
					std::cout << "Header: [" << headerAsString(it->first) << ": " << it->second << "]" << std::endl;
			}
			else
				std::cout << "-NO HEADERS-" << std::endl;
			if (x._body.length())
				std::cout << std::endl << "-BODY-" << std::endl << x._body << std::endl;
			else
				std::cout << std::endl << "-NO BODY-" << std::endl;
		return o;
	}
}

using namespace NotApache;


Request::Request() {
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_methodMap["DELETE"] = DELETE;
	_methodMap["CONNECT"] = CONNECT;
	_methodMap["OPTIONS"] = OPTIONS;
	_methodMap["TRACE"] = TRACE;
	_headerMap["ACCEPT-CHARSET"] = ACCEPT_CHARSET;
	_headerMap["ACCEPT-LANGUAGE"] = ACCEPT_LANGUAGE;
	_headerMap["ALLOW"] = ALLOW;
	_headerMap["AUTHORIZATION"] = AUTHORIZATION;
	//_headerMap["CONNECTION"] = CONNECTION;
	_headerMap["CONTENT-LANGUAGE"] = CONTENT_LANGUAGE;
	_headerMap["CONTENT-LENGTH"] = CONTENT_LENGTH;
	_headerMap["CONTENT-LOCATION"] = CONTENT_LOCATION;
	_headerMap["CONTENT-TYPE"] = CONTENT_TYPE;
	_headerMap["DATE"] = DATE;
	_headerMap["HOST"] = HOST;
	_headerMap["LAST-MODIFIED"] = LAST_MODIFIED;
	_headerMap["LOCATION"] = LOCATION;
	_headerMap["REFERER"] = REFERER;
	_headerMap["RETRY-AFTER"] = RETRY_AFTER;
	_headerMap["SERVER"] = SERVER;
	_headerMap["TRANSFER-ENCODING"] = TRANSFER_ENCODING;
	_headerMap["USER-AGENT"] = USER_AGENT;
	_headerMap["WWW-AUTHENTICATE"] = WWW_AUTHENTICATE;
}

Request::e_ret		Request::parseBody(std::string line) {
	_body = line;
	return OK;
}

Request::e_ret		Request::parseHeaders(std::string line) {
	std::vector<std::string> headers = ft::split(line, "\r\n");

	for (size_t i = 0; i < headers.size(); ++i) {
		std::string fieldName = headers[i].substr(0, headers[i].find_first_of(":"));
		for (size_t i = 0; fieldName[i]; ++i)
			fieldName[i] = std::toupper(fieldName[i]);
		if (_headerMap.find(fieldName) == _headerMap.end())
			continue ; // Header not handled
		else
			_headers[_headerMap.find(fieldName)->second] = headers[i].substr(headers[i].find_first_not_of(" ", fieldName.length()+1));
			// ^ Set header ^
	}
	return OK;
}


Request::e_ret		Request::parseRequestLine(std::string reqLine) {
    std::vector<std::string> parts = ft::split(reqLine, " ");

	if (_methodMap.find(parts[0]) == _methodMap.end()) {
		std::cout << "Invalid method" << std::endl;
		return ERROR;
	}
	// Set Method
	_method = _methodMap.find(parts[0])->second;

	// Set URI
	_uri = parts[1];

	// Set version
	size_t pos = reqLine.find("HTTP/") + 5;
	_version.first = reqLine[pos] - '0';
	_version.second = reqLine[pos+2] - '0';

	return OK;
}

Request::e_ret		Request::parseRequest(std::string request) {
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield

	EOR = request.find("\r\n");
	if (parseRequestLine(request.substr(0, EOR)))
		return ERROR;
	EOR += 2;

	EOH = request.find("\r\n\r\n", EOR);
	if (EOH == std::string::npos) {
		std::cout << "Request only has request line" << std::endl;
		return OK;
	}
	else if (parseHeaders(request.substr(EOR, EOH-EOR))) 
		return ERROR;
	EOH += 4;

	if (_headers.find(CONTENT_LENGTH) != _headers.end())
		parseBody(request.substr(EOH, ft::stoi(_headers[CONTENT_LENGTH])));
	return OK;
}
