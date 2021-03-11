//
// Created by jelle on 3/3/2021.
//

#include "server/parsers/HTTPParser.hpp"
#include "server/parsers/ft_utils.hpp"

#include <iostream>

using namespace NotApache;

HTTPParser::HTTPParser(): AParser(CONNECTION, "HTTP") {}


int		HTTPParser::formatCheckReqLine(const std::string& reqLine) const {
	// CHECKING GLOBAL FORMAT
	size_t spaces = ft::countWS(reqLine);
    std::vector<std::string> parts = ft::split(reqLine, " ");
	if (spaces != 2 || parts.size() != 3) {
		std::cout << "Invalid request line" << std::endl;
		return INVALID;
	}
	
	// CHECK METHOD
	size_t i = 0;
	for (; parts[0][i]; ++i) {
		if (!std::isupper(parts[0][i])) {
			std::cout << "Method format error" << std::endl;
			return INVALID;
		}
	}

	// CHECK URI
	if (parts[1][0] != '/') {
		std::cout << "Invalid uri" << std::endl;
		return INVALID;
	}
	
	// CHECK PROTOCOL
	size_t pos = parts[2].find("HTTP/");
	if (pos == std::string::npos) {
		std::cout << "Invalid protocol" << std::endl;
		return INVALID;
	}
	
	// CHECK VERSION
	pos += 5;
	if (!std::isdigit(parts[2][pos]) && parts[2][pos+1] != '.' && !std::isdigit(parts[2][pos+2])){
		std::cout << "Invalid version format" << std::endl;
		return INVALID;
	}

	return VALID;
}

int		HTTPParser::formatCheckHeaders(const std::string& line) const {
	int ret = VALID;
	std::vector<std::string> headers = ft::split(line, "\r\n");

	for (size_t i = 0; i < headers.size(); ++i) {
		if (headers[i].find(":") == std::string::npos) {
			std::cout << "no \":\" in header line" << std::endl;
			return INVALID;
		}
		//Check for spaces in field-name
		if (ft::countWS(headers[i].substr(0, headers[i].find_first_of(":")))) {
			std::cout << "Spaces in field-name" << std::endl;
			return INVALID;
		}
		
		if (headers[i].find("TRANSFER-ENCODING:", 0, 18) != std::string::npos && headers[i].find("chunked") != std::string::npos) {
			std::cout << "Chunked" << std::endl;
			ret = CHUNKED;
		}
		
		if (headers[i].find("CONTENT-LENGTH:", 0, 15) != std::string::npos) {
			if (ret == CHUNKED) {
				std::cout << "Headers Transfer-encoding + Content-length not allowed" << std::endl;
				return INVALID; //Can't have both CONTENT-LENGTH and TRANSFER-ENCODING
			}
			else
				ret = BODY;
		}
	}
	return ret;
}

int		HTTPParser::formatCheckBody(const std::string& body) const {
	(void)body;
	return VALID;
}

AParser::formatState HTTPParser::formatCheck(Client &client) const {
    AParser::formatState ret = FINISHED;
	size_t EOR = 0; // End Of Requestline
	size_t EOH = 0; // End Of Headerfield
	size_t EOB = 0; // End Of Body

	const std::string   &request = client.getRequest();

	EOR = request.find("\r\n");
	if (EOR == std::string::npos) {
		std::cout << "No \"\r\n\" in request" << std::endl;
		return UNFINISHED;
	}

	EOR += 2;
	if (formatCheckReqLine(request.substr(0, EOR)))
		return PARSE_ERROR;
	
	EOH = request.find("\r\n\r\n", EOR);
	if (EOH == std::string::npos) {
		std::cout << "Request line only" << std::endl;
		return FINISHED;
	}

	int check = formatCheckHeaders(request.substr(EOR, EOH - EOR));
	if (check == INVALID) {
		std::cout << "Invalid header format" << std::endl;
		return PARSE_ERROR;
	}
	else if (check == CHUNKED) {
			EOB = request.find_last_of("\r\n", request.find_last_of("\r\n")-2)+1;
			size_t end = request.find_last_of("\r\n")-1;
			size_t bodySize = ft::stoi(request.substr(EOB, end-EOB));
		if (bodySize != 0) {
			std::cout << "UNFINISHED" << std::endl;
			ret = UNFINISHED;
		}
	}
	else if (check == BODY)
		if (formatCheckBody(request.substr(EOH))) {
			std::cout << "Invalid body format" << std::endl;
			return PARSE_ERROR;
		}

	return ret;
}
