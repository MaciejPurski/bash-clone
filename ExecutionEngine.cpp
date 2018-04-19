#include "ExecutionEngine.h"

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {


}


bool ExecutionEngine::isBuiltIn() {
	return false;
}

int ExecutionEngine::execCommand(ExecutableCommand *command) {
	return 0;
}

void ExecutionEngine::executeCommandLine(CommandLine &line) {

}

void ExecutionEngine::sendSignal(int signal) {

}
