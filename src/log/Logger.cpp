//
// Created by jelle on 3/1/2021.
//

#include <string>
#include "config/ConfigException.hpp"
#include "log/Logger.hpp"

using namespace logger;

Logger::Logger(std::ostream &stream) {
	_streams.push_back(&stream);
}

Logger::Logger(const std::vector<std::ostream*> &streams) {
	_streams = streams;
}

Logger::Logger(const Logger &logger) {
	_streams = logger._streams;
}

void    Logger::log(const LogItem &item) {
	if (item.logType == DEBUG && !(_loggerFlags & Flags::Debug)) return;
	_lock.lock();
	try {
		for (std::vector<std::ostream*>::const_iterator first = _streams.begin(); first != _streams.end(); ++first) {
			**first << item.toString(_loggerFlags) << std::endl;
		}
	} catch (std::exception &e) {}
	_lock.unlock();
}

void    Logger::log(const LogItem &item, const config::ConfigException &e) {
	if (item.logType == DEBUG && !(_loggerFlags & Flags::Debug)) return;
	_lock.lock();
	try {
		for (std::vector<std::ostream*>::const_iterator first = _streams.begin(); first != _streams.end(); ++first) {
			**first << item.toString(_loggerFlags) << "L:" << e.getLine().getLineNumber() <<  " " << e.prettyPrint() << std::endl;
		}
	} catch (std::exception &e) {}
	_lock.unlock();
}
Logger &Logger::operator=(const Logger &rhs) {
	_streams = rhs._streams;
	return *this;
}

void Logger::setFlags(Flags::flagType flags) {
	_loggerFlags = flags;
}
