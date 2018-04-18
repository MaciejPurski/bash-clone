#include "ExecutionEngine.h"

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {

	/*  TODO: Signal handling. Bash should pass the signals to
	 * its child processes
	 *
	 */

}


bool ExecutionEngine::isBuiltIn() {
	return false;
}

int ExecutionEngine::execCommand(ExecutableCommand *command) {
	return 0;
}

void ExecutionEngine::executeCommandLine(CommandLine &line) {

}
