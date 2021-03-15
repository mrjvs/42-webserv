//
// Created by jelle on 3/2/2021.
//

#include "server/Client.hpp"
#include <unistd.h>

using namespace NotApache;

Client::Client(FD readFD, FD writeFD, ClientTypes type): _readFD(readFD), _writeFD(writeFD), _type(type), _state(READING),
_dataType(), _rawRequest(), _response(), _responseIndex(), _responseState(), _created(), _timeoutSeconds(0), _request() {
	::gettimeofday(&_created, NULL);
}

FD Client::getReadFD() const {
	return _readFD;
}

FD Client::getWriteFD() const {
	return _writeFD;
}

void Client::setWriteFD(FD fd) {
	_writeFD = fd;
}

void Client::setReadFD(FD fd) {
	_readFD = fd;
}

ClientStates Client::getState() const {
	return _state;
}

void Client::setState(ClientStates state) {
	_state = state;
}

void Client::appendRequest(const std::string &str) {
	_rawRequest += str;
}

std::string Client::getRawRequest() const {
	return _rawRequest;
}

Request& Client::getRequest() {
	return _request;
}

void Client::setRawRequest(const std::string &str) {
	_rawRequest = str;
}

ClientTypes Client::getType() const {
	return _type;
}

std::string Client::getDataType() const {
	return _dataType;
}

std::string Client::getResponse() const {
	return _response;
}

std::size_t Client::getResponseIndex() const {
	return _responseIndex;
}

void Client::setDataType(const std::string &str) {
	_dataType = str;
}

void Client::appendResponse(const std::string &str) {
	_response += str;
}

void Client::setResponse(const std::string &str) {
	_response = str;
}

void Client::setResponseIndex(std::size_t i) {
	_responseIndex = i;
}

ResponseStates Client::getResponseState() const {
	return _responseState;
}

void Client::setResponseState(ResponseStates state) {
	_responseState = state;
}

void Client::close(bool reachedEOF) {
	(void)reachedEOF;
	::close(_readFD);
	_state = CLOSED;
}

Client::~Client() {}

ssize_t Client::read(char *buf, size_t len) {
	return ::read(_readFD, buf, len);
}

ssize_t Client::write(const char *buf, size_t len) {
	return ::write(_writeFD, buf, len);
}

void Client::setTimeout(unsigned long seconds) {
	_timeoutSeconds = seconds;
}

void Client::timeout() {
	timeval	curTime;
	::gettimeofday(&curTime, NULL);
	if (curTime.tv_sec < _created.tv_sec + (time_t)_timeoutSeconds) return;

	// do timeout
	setResponseState(TIMED_OUT);
	setState(WRITING);
	// set datatype to HTTP so it responds with a correct format
	setDataType("HTTP");
}

const timeval &Client::getCreatedAt() const {
	return _created;
}

void	Client::setRequest() {
	_request.parseRequest(_rawRequest);
}
