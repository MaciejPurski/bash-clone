#include "ExecutionEngine.h"
#include <csignal>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {
	nextJobNumber = 1;
}

void ExecutionEngine::executeBuiltIn(Command &command) {
	if (command.command == "pwd") {
		std::cout << environment.getCurrentDir() << std::endl;
	} else if (command.command == "cd") {
		cdCommand(command);
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
	} else if (command.command == "export") {
		exportCommand(command);
	} else {
		throw std::runtime_error("Unkown built-in");
	}
}

std::string ExecutionEngine::checkCommand(Command &command) {
	std::string fullPath = environment.resolveCommand(command.command);

	if (access(fullPath.c_str(), F_OK))
		throw std::runtime_error("File does not exist");

	if (access(fullPath.c_str(), R_OK))
		throw std::runtime_error("User does not have read permission");

	if (access(fullPath.c_str(), X_OK))
		throw std::runtime_error("User does not have execute permission");

	command.setFullPath(fullPath);
	return fullPath;
}

void ExecutionEngine::executeCommandLine(std::vector<Command> &commands) {
	std::vector<Command> pipe;

	for (auto &cmd : commands) {
		if (cmd.command.empty())
			continue;
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
			addJob(pipe);
			pipe.clear();
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

	std::string expanded = environment.expandPath(command.args[0]);

	if (!environment.checkIfDirExists(expanded)) {
		throw std::runtime_error("Directory does not exist");
	}

	environment.setCurrentDir(expanded);
}

void ExecutionEngine::envCommand(Command &command) {
	char **env = environment.getEnvironment();

	for (int i = 0; env[i] != nullptr; i++) {
		std::cout << env[i] << std::endl;
		delete[] env[i];
	}

	delete[] env;
}

void ExecutionEngine::jobsCommand() {
	/* Update background jobs state */
	update();

	for (auto it = jobs.rbegin(); it != jobs.rend(); it++)
		it->showJob();

	jobsCleanup();
}

void ExecutionEngine::showDoneBackgroundJobs() {
	for (auto it = jobs.rbegin(); it != jobs.rend(); it++)
		if (it->isDone() && it->isBackground())
			it->showJob();
}

void ExecutionEngine::update() {
	for (auto &job : jobs)
		job.updateState();
}

void ExecutionEngine::jobsCleanup() {
	for (auto it = jobs.begin(); it != jobs.end(); it++) {
		if (it->isDone()) {
			jobs.erase(it);

			/* we need to call it recursively as the iterator is invalid */
			jobsCleanup();
			break;
		}
	}

}

void ExecutionEngine::exportCommand(Command &command) {
	for (auto &arg : command.args) {
		/* get variable name */
		std::string valName;
		int i = 0;

		while (i < arg.size() && arg[i] != '=') {
			valName += arg[i++];
		}

		if (i != arg.size()) {
			std::string value;

			while (i < arg.size())
				value += arg[i++];

			environment.setVariable(valName, value);
		}

		environment.exportVariable(valName);
	}
}

void ExecutionEngine::addJob(std::vector<Command> &pipe) {
	if (jobs.empty())
		nextJobNumber = 1;

	jobs.push_front(Job(pipe, nextJobNumber++));
	jobs.front().start(environment);
}

