#ifndef BASH_CLONE_LEXER_H
#define BASH_CLONE_LEXER_H

#include <vector>
#include "Token.h"
#include <string>

/**
 *	@class Lexer
 *
 *	@brief Used by Interpreter lexical analysis
 */
class Lexer {

public:
    /**
	 * @brief Parses commands, changes string into vector of Tokens
	 * @param line string which needs analyze
	 * @return vector of Tokens
	 */
    std::vector<Token> separateTokens(std::string &line);

private:
    /**
	 * @brief Checks if sign is on list of signs, used to check if the sign
     * is one of the given signs
	 * @param sign char from analyzed line
     * @param acceptable list of signs
	 * @return true if sign is on list, false if isn't
	 */
    bool checkSign(char sign, const std::initializer_list<char> &acceptable) const;
};


#endif //BASH_CLONE_LEXER_H
