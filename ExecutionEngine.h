#ifndef BASH_CLONE_ENGINE_H
#define BASH_CLONE_ENGINE_H

#include "Interpreter.h"
#include "Environment.h"


/**
 * @class Engine
 * @brief Main bash engine class.
 *
 * Implements main program loop, which
 * fetches commands from the interpreter and executes commands.
 *
 * It implements both interactive and non-interactive mode. In
 * interactive mode the program shows encourage symbol and waits for user's input.
 * In non-interactive mode it executes all commands until the EOF is reached
 * and finishes execution.
 */
class ExecutionEngine {
private:
	Environment &environment;

	bool isBuiltIn();
	int execCommand(ExecutableCommand *command);

public:
	ExecutionEngine(Environment &e);
	void executeCommandLine(CommandLine &line);
	void sendSignal(int signal);

};


#endif //BASH_CLONE_ENGINE_H
