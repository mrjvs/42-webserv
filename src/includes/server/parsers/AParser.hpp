//
// Created by jelle on 3/3/2021.
//

#ifndef APARSER_HPP
#define APARSER_HPP

#include "server/Client.hpp"
#include <vector>

namespace NotApache {

	class Client;

	class AParser {
	protected:
		ClientTypes	_type;
		std::string	_dataType;

	public:
		enum formatState {
			UNFINISHED,
			FINISHED,
			PARSE_ERROR
		};

		AParser(ClientTypes type, const std::string &dataType);
		virtual ~AParser();

		virtual ClientTypes	getType() const;
		virtual std::string	getDataType() const;
		virtual void 		setType(ClientTypes type);

		virtual formatState	formatCheck(Client &client) const = 0;

		static formatState	runFormatChecks(std::vector<AParser *> &parsers, Client &client);
	};
}

#endif //APARSER_HPP
