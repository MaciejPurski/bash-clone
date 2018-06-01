#ifndef BASH_CLONE_ENGINE_H
#define BASH_CLONE_ENGINE_H

#include "Environment.h"
#include "Command.h"

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
    bool executeBuiltIn(Command &command);
    int execCommand(Command &command);

public:
    ExecutionEngine(Environment &e);
    void executeCommandLine(std::vector<Command> commands);
	void sendSignal(int signal);

	void cdCommand(Command &command);
};


#endif //BASH_CLONE_ENGINE_H
