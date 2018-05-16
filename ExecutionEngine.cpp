#include "ExecutionEngine.h"

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {


}


bool ExecutionEngine::isBuiltIn() {
	return false;
}

int ExecutionEngine::execCommand(Command *command) {
	return 0;
}


void ExecutionEngine::sendSignal(int signal) {

}
