#include "ExecutionEngine.h"

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {

}

bool ExecutionEngine::executeBuiltIn(Command &command) {
	if (command.command == "echo") {
		for (auto &arg : command.args)
			std::cout << arg << " ";

		std::cout << std::endl;
		return true;
	} else if (command.command == "pwd") {
		std::cout << environment.getCurrentDir() << std::endl;
	} else if (command.command == "cd") {

	} else if (command.command == "kill") {

	} else if (command.command == "env") {

	} else if (command.command == "fg") {

	} else if (command.command == "bg") {

	} else if (command.command == "jobs") {

	} else {
		return false;
	}
}

void ExecutionEngine::sendSignal(int signal) {

}

int ExecutionEngine::execCommand(Command &command) {
	executeBuiltIn(command);
	return 0;
}

void ExecutionEngine::executeCommandLine(std::vector<Command> commands) {
	for (auto c : commands) {
		execCommand(c);
	}
}
