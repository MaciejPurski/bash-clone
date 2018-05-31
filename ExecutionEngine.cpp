#include "ExecutionEngine.h"

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {

}

bool ExecutionEngine::isBuiltIn() {
	return false;
}

void ExecutionEngine::sendSignal(int signal) {

}

int ExecutionEngine::execCommand(Command *command) {
	return 0;
}

void ExecutionEngine::executeCommandLine(std::vector<Command> commands) {

}
