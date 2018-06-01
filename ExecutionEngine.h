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
	enum Direction {IN, OUT};
	Environment &environment;
    bool executeBuiltIn(Command &command);
    int execCommand(Command &command);
	void cdCommand(Command &command);
	void killCommand(Command &command);
	void envCommand(Command &command);
	void changeProcessImage(Command &command, std::string fullPath);
	void handleRedirection(Command::Redirection &redirection);
	std::string pipeOpen(std::string src);

public:
    ExecutionEngine(Environment &e);
    void executeCommandLine(std::vector<Command> commands);
	void sendSignal(int signal);

};


#endif //BASH_CLONE_ENGINE_H
