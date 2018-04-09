#include "Interpreter.h"

Interpreter::Interpreter() {

}

void Interpreter::openFile(const std::string &fName) {

}

CommandLine Interpreter::nextCommandLine() {
	// just for testing the polimorphism
	CommandLine n;
	n.push_back(new AssignCommand());
	n.push_back(new ExecutableCommand());

	return n;
}
