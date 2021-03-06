//
// Created by jelle on 3/8/2021.
//

#ifndef ACONFIGBLOCK_HPP
#define ACONFIGBLOCK_HPP

#include "config/ConfigLine.hpp"
#include "config/AConfigValidator.hpp"
#include <vector>
#include <exception>
#include <map>

namespace config {

	class AConfigBlock {
	public:
		typedef	std::vector<const AConfigValidator *>		validatorListType;
		typedef std::map<std::string, validatorListType>	validatorsMapType;

	protected:
		std::vector<ConfigLine>		_lines;
		std::vector<AConfigBlock*>	_blocks;
		AConfigBlock				*_parent;
		int 						_lineNumber;
		bool						_isParsed;

		virtual const validatorsMapType	&getValidators() const = 0;
		virtual const validatorListType	&getBlockValidators() const = 0;
		virtual const std::string		*getAllowedBlocks() const = 0;
		const validatorListType			&getValidatorForKey(const ConfigLine &line) const;
		void 							runValidatorForKey(const ConfigLine &line) const;

	public:
		AConfigBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent = 0);
		virtual ~AConfigBlock();

		static void validateEndBlock(const ConfigLine &line);
		virtual std::string getType() const = 0;
		virtual void 		parseData() = 0;

		void	addLine(const ConfigLine &line);
		void	addBlock(AConfigBlock *block);
		bool 	hasKey(const std::string &key) const;
		const ConfigLine *getKey(const std::string &key) const;
		int 	getLineNumber() const;
		AConfigBlock	*getParent() const;

		void	runPostValidators();
		void 	throwNotParsed() const;

		class NotParsedException: public std::exception {
			const char * what() const throw() {
				return "Configuration must be parsed before usage";
			}
		};

	};

}

#endif //ACONFIGBLOCK_HPP
