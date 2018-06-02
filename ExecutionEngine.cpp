#include "ExecutionEngine.h"
#include <csignal>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {

}


void ExecutionEngine::executeBuiltIn(Command &command) {
	if (command.command == "pwd") {
		std::cout << environment.getCurrentDir() << std::endl;
	} else if (command.command == "cd") {
		cdCommand(command);
	} else if (command.command == "kill") {
		killCommand(command);
	} else if (command.command == "env") {
		envCommand(command);
	} else if (command.command == "fg") {
		if (!jobs.empty() && !jobs.front().isDone())
			jobs.front().runForeground(jobs.front().isStopped());
	} else if (command.command == "bg") {
		if (!jobs.empty() && !jobs.front().isDone())
			jobs.front().runBackground(true);
	} else if (command.command == "jobs") {
		jobsCommand();
	} else {
		throw std::runtime_error("Unkown built-in");
	}
}

void ExecutionEngine::sendSignal(int signal) {

}

int ExecutionEngine::execCommand(Command &command) {



	//TODO: set return code

	return 0;
}

std::string ExecutionEngine::checkCommand(Command &command) {
	std::string fullPath;

	// if the command path is not relative
	if (command.command[0] == '/' || command.command[0] == '.' ||
	    command.command[0] == '~' || command.command.substr(0, 2) == "..") {
		fullPath = environment.expandPath(command.command);
	} else {
		fullPath = environment.searchPath(command.command);
		if (fullPath.empty())
			throw std::runtime_error("Can't find command: " + command.command);
	}

	if (access(fullPath.c_str(), F_OK))
		throw std::runtime_error("File does not exist");

	if (access(fullPath.c_str(), R_OK))
		throw std::runtime_error("User does not have read permission");

	if (access(fullPath.c_str(), X_OK))
		throw std::runtime_error("User does not have execute permission");

	command.setFullPath(fullPath);
	return fullPath;
}

void ExecutionEngine::executeCommandLine(std::vector<Command> commands) {
	std::vector<Command> pipe;

	for (auto &cmd : commands) {
		/* The built in, which is &-terminated or is a part of a pipe
		 * shall be ignored
		 */
		if (cmd.isBuiltIn()) {
			if (!cmd.pipeTerminated() && !cmd.isBackgroud() && pipe.empty())
				executeBuiltIn(cmd);

			continue;
		}

		checkCommand(cmd);

		/* Collect processes to form a pipeline */
		pipe.push_back(cmd);

		/* Finish the pipeline by adding it to jobs list */
		if (!cmd.pipeTerminated()) {
			int jobNumber;

			if (jobs.empty())
				jobNumber = 1;
			else
				jobNumber = jobs.front().getNumber() + 1;

			jobs.push_front(Job(pipe, jobNumber));
			pipe.clear();

			jobs.front().start(environment.getCurrentDir());
		}
	}
}

void ExecutionEngine::cdCommand(Command &command) {
	if (command.args.empty()) {
		environment.setCurrentDir(environment.getValue("HOME"));
		return;
	}
	if (command.args.size() > 1) {
		throw std::runtime_error("cd command expects one argument");
	}

	if (!environment.checkIfDirExists(command.args[0])) {
		throw std::runtime_error("Directory does not exist");
	}

	environment.setCurrentDir(environment.expandPath(command.args[0]));
}

void ExecutionEngine::killCommand(Command &command) {

}

void ExecutionEngine::envCommand(Command &command) {
	char **env = environment.getEnvironment();

	for (int i = 0; env[i] != NULL; i++) {
		std::cout << env[i] << std::endl;
		delete[] env[i];
	}

	delete[] env;
}

void ExecutionEngine::jobsCommand() {
	for (auto it = jobs.rbegin(); it != jobs.rend(); it++) {
		std::cout << "[" << it->getNumber() << "]" << " " << it->getPid() << " " << it->getState() << " " << it->getCommandLine();
		std::cout << std::endl;
	}
}



