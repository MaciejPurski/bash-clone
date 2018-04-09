#include "Engine.h"

Engine::Engine(Interpreter &i, Environment &e, bool interactiveMode) : interpreter(i), environment(e) {
	isInteractiveMode = interactiveMode;

	/*  TODO: Signal handling. Bash should pass the signals to
	 * its child processes
	 *
	 */

}

void Engine::run() {
	CommandLine cl = interpreter.nextCommandLine();

	for (Command *c : cl) {
		if (c->getType() == Command::ASSIGN) {
			// avoid ineffective dynamic_cast
			auto a = static_cast<AssignCommand*> (c);

			std::cout << "assign" << std::endl;

		} else {
			auto e = static_cast<ExecutableCommand*> (c);

			execCommand(e);

		}
	}

}

bool Engine::isBuiltIn() {
	return false;
}

int Engine::execCommand(ExecutableCommand *command) {
	return 0;
}