//
// Created by pmerku on 14/04/2021.
//

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <string>
#include "server/global/GlobalLogger.hpp"

namespace NotApache {
	class HTTPClient;
}

namespace plugin {


	class Plugin {
	private:
		std::string	_id;

	public:
		explicit Plugin(const std::string &name);
		virtual ~Plugin();

		virtual bool onHandleError(NotApache::HTTPClient &client, int code);
		virtual bool onFileServing(NotApache::HTTPClient &client);
		virtual bool onSendFile(NotApache::HTTPClient &client);

		const std::string &getId() const;

		bool operator==(const std::string &name) const;
		bool operator==(const char *name) const;
	};

} // namespace plugin

#endif //PLUGIN_HPP
