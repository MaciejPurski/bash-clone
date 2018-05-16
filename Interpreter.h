#ifndef BASH_CLONE_INTERPRETER_H
#define BASH_CLONE_INTERPRETER_H
#include <iostream>
#include <vector>
#include "Command.h"
#include "Environment.h"
#include "ExecutionEngine.h"

typedef std::vector<Command*> CommandLine;

/**
 * @class Interpreter
 * @brief Parses input.
 *
 * By default it works on stdin, but it might work on file
 * input as well.
 */
class Interpreter {
private:
	Environment &env;
	ExecutionEngine &engine;

	enum TokenType {QUOTATION, PIPE, AMPERSAND, SEMICOLON, END, STREAM, SPACE, ASSIGNMENT, WITH$, WORD};

	struct Token{
		Token(TokenType type, const std::string &value);

		TokenType type;
		std::string value;
	};

	std::vector<Token> separateTokens(std::string &line);
	std::vector<std::vector<Token>> separateInstruction(std::vector<Token> &tokens);
	void interpretInstruction(std::vector<Token> &instruction);


public:
	Interpreter(Environment &nenv, ExecutionEngine &engine) : env(nenv), engine(engine) { };

	void nextCommandLine(std::string &line);

};


#endif //BASH_CLONE_INTERPRETER_H
