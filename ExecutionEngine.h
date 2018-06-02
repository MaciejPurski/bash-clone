#ifndef BASH_CLONE_ENGINE_H
#define BASH_CLONE_ENGINE_H

#include <stack>
#include "Environment.h"
#include "Command.h"
#include "Job.h"
#include <list>

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
    void executeBuiltIn(Command &command);
    int execCommand(Command &command);
	void cdCommand(Command &command);
	void killCommand(Command &command);
	void envCommand(Command &command);
	void changeProcessImage(Command &command, std::string fullPath, bool foreground);
	void handleRedirection(Command::Redirection &redirection);
	std::string pipeOpen(std::string src);
	std::deque<Job> jobs;

public:
    ExecutionEngine(Environment &e);
    void executeCommandLine(std::vector<Command> commands);
	void sendSignal(int signal);

	std::string checkCommand(const Command &command);

	void pipeProcess(std::vector<Command> &commands);

	std::string checkCommand(Command &command);

	void jobsCommand();
};


#endif //BASH_CLONE_ENGINE_H
