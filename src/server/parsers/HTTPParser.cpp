//
// Created by jelle on 3/3/2021.
//

#include "server/parsers/HTTPParser.hpp"

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
            default:
                return "INVALID METHOD";
        }
    }

	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& x) {
		for (size_t i = 0; i < x.size(); ++i)
			o << x[i] << std::endl;
		return o;
	}
}

using namespace NotApache;

std::vector<std::string>        split(const std::string& str, const std::string& del)
{
    size_t start = 0, end = 0;
    std::vector<std::string> array;

    while (end != str.npos)
    {
        start = str.find_first_not_of(del, end);
        end = str.find_first_of(del, start);
		if (end != std::string::npos || start != std::string::npos)
        	array.push_back(str.substr(start, end - start));
    }
    return array;
}

HTTPParser::HTTPParser(): AParser(CONNECTION, "HTTP") {
	methodMap["GET"] = GET; // These are mandatory according to rfc's
    methodMap["HEAD"] = HEAD; // These are mandatory according to rfc's
    methodMap["PUT"] = PUT;
    methodMap["DELETE"] = DELETE;
    methodMap["CONNECT"] = CONNECT;
    methodMap["OPTIONS"] = OPTIONS;
    methodMap["TRACE"] = TRACE;
    methodMap["ERROR"] = ERROR;
}

int		HTTPParser::parseRequestLine(std::string& reqLine) const {
    std::vector<std::string> parts = split(reqLine, " ");
	if (parts.size() != 3)
		return INVALID; //ENUM ERROR
	
	// CHECK METHOD
	//if (parts[0].length() > 7) check len here?
	if (methodMap.find(parts[0]) == methodMap.end())
		return INVALID; //ENUM ERROR
	
	if (parts[1][0] != '/')
		return INVALID; //ENUM ERROR
	
	if (parts[2] != HTTP_VERSION) //TYPEDEF
		return INVALID; //ENUM ERROR
	return VALID;
}

AParser::formatState HTTPParser::formatCheck(Client &client) const {
    const std::string   expected = "GET / HTTP/1.1\n";
    //const std::string   &request = client.getRequest();
	(void)client;
    const std::string   &request = "\nGET / HTTP/1.1\r\n";

	std::cout << request << std::endl;
    std::vector<std::string> line = split(request, "\r\n");
	
	if (parseRequestLine(line[0])) {
		std::cout << "ERRORTJE\n";
		return PARSE_ERROR;
	}

	//std::cout << parts << std::endl;




	//this->methodMap.at(parts[0]);

	// CHECK


    for (std::string::size_type i = 0; i < request.length(); ++i) {
        if (i >= expected.length())
            return FINISHED;
        if (request[i] != expected[i])
            return PARSE_ERROR;
    }

    if (expected.length() == request.length())
        return FINISHED;
    return UNFINISHED;
}



/*
    GET / HTTP/1.1\r\n
    
    request-line   = method SP request-target SP HTTP-version CRLF
    
    A request-line begins with a method token, followed by a single space
    (SP), the request-target, another single space (SP), the protocol
    version, and ends with CRLF (\r\n)

   +---------+-------------------------------------------------+-------+
   | Method  | Description                                     | Sec.  |
   +---------+-------------------------------------------------+-------+
   | GET     | Transfer a current representation of the target | 4.3.1 |
   |         | resource.                                       |       |
   | HEAD    | Same as GET, but only transfer the status line  | 4.3.2 |
   |         | and header section.                             |       |
   | POST    | Perform resource-specific processing on the     | 4.3.3 |
   |         | request payload.                                |       |
   | PUT     | Replace all current representations of the      | 4.3.4 |
   |         | target resource with the request payload.       |       |
   | DELETE  | Remove all current representations of the       | 4.3.5 |
   |         | target resource.                                |       |
   | CONNECT | Establish a tunnel to the server identified by  | 4.3.6 |
   |         | the target resource.                            |       |
   | OPTIONS | Describe the communication options for the      | 4.3.7 |
   |         | target resource.                                |       |
   | TRACE   | Perform a message loop-back test along the path | 4.3.8 |
   |         | to the target resource.                         |       |
   +---------+-------------------------------------------------+-------+



	If terminating the request message body with a line-ending is desired, then the user agent MUST
	count the terminating CRLF octets as part of the message body length.

	In the interest of robustness, a server that is expecting to receive
   and parse a request-line SHOULD ignore at least one empty line (CRLF)
   received prior to the request-line.


    Although the request-line and status-line grammar rules require that
   each of the component elements be separated by a single SP octet,
   recipients MAY instead parse on whitespace-delimited word boundaries
   and, aside from the CRLF terminator, treat any form of whitespace as
   the SP separator while ignoring preceding or trailing whitespace;
   such whitespace includes one or more of the following octets: SP,
   HTAB, VT (%x0B), FF (%x0C), or bare CR.


   A server that receives a request-target longer than any URI it wishes to parse MUST respond
   with a 414 (URI Too Long) status code
*/

