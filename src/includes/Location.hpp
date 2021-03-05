#pragma once

# include <vector>
# include <string>

class Location
{
	public:
		Location();

		Location&	operator=(const Location& rhs);

		void		setPrefix(const std::vector<std::string>& args);
		void		setRoot(const std::vector<std::string>& args);
		void		setIndex(const std::vector<std::string>& args);
		void		setAutoindex(const std::vector<std::string>& args);
		void		setMaxBody(const std::vector<std::string>& args);
		void		setAllow_method(const std::vector<std::string>& args);

		std::string					getPrefix() const;
		std::string					getRoot() const;
		std::string					getAutoindex() const;
		std::vector<std::string>	getIndex() const;
		std::vector<std::string>	getAllow_method() const;
		size_t						getMaxBody() const;

	private:
		std::string					_prefix,
									_root,
									_autoindex,
									_default_cgi_path,
									_php_cgi;
		std::vector<std::string>	_allow_method; //
		std::vector<std::string>	_index;
		size_t						_maxBody;
};

std::ostream&		operator<<(std::ostream& o, const Location& x);

std::ostream&		operator<<(std::ostream& o, const std::vector<std::string>& x);
