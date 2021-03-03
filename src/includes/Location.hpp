#pragma once

# include <vector>
# include <string>

class Location
{
	public:
		Location();
		Location(const Location& rhs);
		~Location();

		Location&	operator=(const Location& rhs);

		void		setPrefix(const std::string& str);
		void		setRoot(const std::string& str);
		void		setIndex(const std::string& str);
		void		setAutoindex(const std::string& str);
		void		setMaxBody(const std::string& str);
		void		setAllow_method(const std::string& str);

		std::string					getPrefix() const;
		std::string					getRoot() const;
		std::string					getAutoindex() const;
		std::vector<std::string>	getIndex() const;
		std::vector<std::string>	getAllow_method() const;
		size_t						getMaxBody() const;

	private:
		std::string					_prefix,
									_root,
									_autoindex;
		std::vector<std::string>	_allow_method; //
		std::vector<std::string>	_index;
		size_t						_maxBody;
};

std::ostream&		operator<<(std::ostream& o, const Location& x);

std::ostream&		operator<<(std::ostream& o, const std::vector<std::string>& x);

// template <class C>
// std::ostream&		operator<<(std::ostream& o, const std::vector<C>& x);
