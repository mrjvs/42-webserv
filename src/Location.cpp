#include "includes/Location.hpp"
#include "includes/ft_utils.hpp"

#include <iostream>

Location::Location() : _maxBody(0) {}

void		Location::setPrefix(const std::vector<std::string>& args) {
	if (args.size() != 3 || args[1][0] != '/')
		throw std::runtime_error("prefix invalid");
	this->_prefix = args[1];
}

void		Location::setRoot(const std::vector<std::string>& args) {
	if (args.size() != 2)
		throw std::runtime_error("root invalid");
	this->_root = args[1];
}

void		Location::setIndex(const std::vector<std::string>& args) {
	this->_index = args;
}

void		Location::setAutoindex(const std::vector<std::string>& args) {
	if (args.size() != 2)
		throw std::runtime_error("Autoindex invalid");
	this->_autoindex = args[1];
}

void		Location::setMaxBody(const std::vector<std::string>& args) {
	if (args.size() != 2)
		throw std::runtime_error("maxBody invalid");
	this->_maxBody = ft::stoi(args[1]);
	if (!this->_maxBody)
		throw std::runtime_error("maxBody invalid");
}

void		Location::setAllow_method(const std::vector<std::string>& args) {
	this->_allow_method.assign(args.begin()+1, args.end());
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
		<<	"\tAllow methods: "		<< x.getAllow_method()	<< std::endl;
	return o;
}

std::ostream&		operator<<(std::ostream& o, const std::vector<std::string>& x)
{
	for (size_t i = 0; i < x.size(); ++i)
		o << x[i] << ' ';
	return o;
}
