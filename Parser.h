#ifndef BASH_CLONE_PARSER_H
#define BASH_CLONE_PARSER_H

#include "Token.h"
#include "Command.h"
#include "Environment.h"

/**
 *	@class Parser
 *
 *	@brief Used by Interpreter to parse commands
 */
class Parser {

public:

    /**
	 * @brief Constructor
     * @param env reference to the environment
	 */
    Parser(Environment &env);

    /**
	 * @brief Parses commands, changes vector of Tokens into vector of Comments
	 * @param tokens vector of Tokens to parse
	 * @return vector of parsed Commands
	 */
    std::vector<Command> separateCommands(std::vector<Token> &tokens);

private:
    Environment &env;

    /**
	 * @brief Separates all tokens to groups of tokens (each creates command) and validates syntax
	 * @param tokens vector of Tokens to parse
	 * @return vector of parsed Commands
	 * @throws InterpreterException if finds syntax error
	 */
    std::vector<std::vector<Token>> syntaxAnalyse(std::vector<Token> &tokens);

    /**
	 * @brief Creates Command from Tokens
	 * @param tokens vector of Tokens
	 * @return Command
	 */
    Command createCommand(std::vector<Token> &tokens);

    /**
	 * @brief Checks if TokenType is on list of TokenTypes, used to check if the token
     * is one of the given types
	 * @param type type of Token
     * @param acceptable list of TokenTypes
	 * @return true if TokenType is on list, false if isn't
	 */
    bool checkType(TokenType type, const std::initializer_list<TokenType> &acceptable) const;

    /**
	 * @brief Concatenates few tokens, which should create one word (we can't create one token in Lexer
     * because sense of expression depends on the context)
	 * @param tokens vector of Tokens
     * @param it iterator which shows the location of the current token
	 * @return created string
	 */
    std::string concatenation(std::vector<Token> &tokens, int &it);

    /**
	 * @brief Parses tokens into Redirection
	 * @param tokens vector of Tokens
     * @param it iterator which shows the location of the current token
	 * @return created Redirection
	 */
    Command::Redirection parseRedirection(std::vector<Token> &tokens, int &it);

    /**
	 * @brief Parses tokens which creates assignment and asks Environment
     * to set the variable to the given value
	 * @param tokens vector of Tokens
     * @param it iterator which shows the location of the current token
	 * @return name of variable
	 */
    std::string parseAssignment(std::vector<Token> &tokens, int &it);
};


#endif //BASH_CLONE_PARSER_H
