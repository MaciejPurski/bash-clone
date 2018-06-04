#ifndef BASH_CLONE_INTERPRETER_H
#define BASH_CLONE_INTERPRETER_H

#include <iostream>
#include <vector>
#include "Environment.h"
#include "Lexer.h"
#include "Parser.h"


/**
 * @class Interpreter
 * @brief Parses input and calls ExecutionEngine to execute command
 *
 * By default it works on stdin, but it might work on file
 * input as well.
 */
class Interpreter {
public:


	Interpreter(Environment &nenv) : env(nenv), parser(nenv), lexer() {};

	/**
	 * processCommandLine
	 * @brief The method processes command line by interpreting it, performing assignments
	 * 		  and variables evaluation
	 * @param line Command line read by the terminal
	 * @return A vector of commands, which should be sent to the execution engine
	 */
	std::vector<Command> processCommandLine(std::string &line);

private:
	Environment &env;
	Lexer lexer;
	Parser parser;

};


#endif //BASH_CLONE_INTERPRETER_H
