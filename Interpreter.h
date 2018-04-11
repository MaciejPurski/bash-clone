#ifndef BASH_CLONE_INTERPRETER_H
#define BASH_CLONE_INTERPRETER_H
#include <iostream>
#include <vector>
#include "Command.h"

typedef std::vector<Command*> CommandLine;

/**
 * @class Interpreter
 * @brief Parses input and returns commandlines.
 *
 * By default it works on stdin, but it might work on file
 * input as well.
 */
class Interpreter {
private:
	std::string fileName;

public:
	Interpreter();

	void openFile(const std::string &fName);
	CommandLine nextCommandLine();

};


#endif //BASH_CLONE_INTERPRETER_H
