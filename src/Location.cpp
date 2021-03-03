#include "includes/Location.hpp"
#include "includes/ft_utils.hpp"

#include <iostream>

Location::Location() : _maxBody(0) {}

Location::Location(const Location& rhs) {
	(void)rhs;
}

Location::~Location() {}

Location&	Location::operator=(const Location& rhs) {
	(void)rhs;
	return *this;
}

void		Location::setPrefix(const std::string& str) {
	std::vector<std::string> tmp = ft::split(str, " \t\n");

	if (tmp.size() > 2 || (tmp.size() == 2 && tmp[0][0] != '/'))
		throw std::runtime_error("prefix invalid");
	if (tmp.size() == 2)
		this->_prefix = tmp[0];
	else
		this->_prefix = "";
}

void		Location::setRoot(const std::string& str) {
	std::vector<std::string> tmp = ft::split(str, " \t\n");
	if (tmp.size() != 1)
		throw std::runtime_error("root invalid");
	this->_root = tmp[0];
}

void		Location::setIndex(const std::string& str) {
	this->_index = ft::split(str, " ");
}

void		Location::setAutoindex(const std::string& str) {
	std::vector<std::string> tmp = ft::split(str, " \t\n");
	if (tmp.size() != 1)
		throw std::runtime_error("Autoindex invalid");
	this->_autoindex = tmp[0];
}

void		Location::setMaxBody(const std::string& str) {
	this->_maxBody = ft::stoi(str);
	if (!this->_maxBody)
		throw std::runtime_error("maxBody invalid");
}

void		Location::setAllow_method(const std::string& str) {
	this->_allow_method = ft::split(str, " ");
}

std::string					Location::getPrefix() const { return this->_prefix; }
std::string					Location::getRoot() const { return this->_root; }
std::string					Location::getAutoindex() const { return this->_autoindex; }
std::vector<std::string>	Location::getIndex() const { return this->_index; }
std::vector<std::string>	Location::getAllow_method() const { return this->_allow_method; }
size_t						Location::getMaxBody() const { return this->_maxBody; }

std::ostream&		operator<<(std::ostream& o, const Location& x)
{
	o	<<	"\nLocation:"									<< std::endl
		<<	"\tPrefix: " 			<< x.getPrefix()		<< std::endl
		<<	"\tRoot: " 				<< x.getRoot()			<< std::endl
		<<	"\tAutoindex: " 		<< x.getAutoindex()		<< std::endl
		<<	"\tIndex: " 			<< x.getIndex()			<< std::endl
		<<	"\tMaxBody: " 			<< x.getMaxBody()		<< std::endl
		<<	"\tAllow methods: "		<< x.getAllow_method();
	return o;
}

std::ostream&		operator<<(std::ostream& o, const std::vector<std::string>& x)
{
	for (size_t i = 0; i < x.size(); ++i)
		o << x[i] << ' ';
	return o;
}
