#include "ExecutionEngine.h"
#include <csignal>
#include <fcntl.h>
#include <wait.h>

ExecutionEngine::ExecutionEngine(Environment &e) : environment(e) {

}

bool ExecutionEngine::executeBuiltIn(Command &command) {
	//if (command.command == "echo") {
		//for (auto &arg : command.args)
		//	std::cout << arg << " ";

		//std::cout << std::endl;
		//return true;
	//} else
	if (command.command == "pwd") {
		std::cout << environment.getCurrentDir() << std::endl;
	} else if (command.command == "cd") {
		cdCommand(command);
		return true;
	} else if (command.command == "kill") {
		killCommand(command);
		return true;
	} else if (command.command == "env") {
		envCommand(command);
		return true;
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
	// first check if the command is built in
	if (executeBuiltIn(command))
		return 0;

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

	int pid;
	if ((pid = fork() == 0)) {
		changeProcessImage(command, fullPath);
	} else if (pid < 0)
		throw std::runtime_error("Problem while forking new process: " + std::string(strerror(errno)));

	fflush(stdout);

	int status;
	waitpid(pid, &status, 0);

	//TODO: set return code

	return 0;
}

void ExecutionEngine::executeCommandLine(std::vector<Command> commands) {
	for (auto c : commands) {
		execCommand(c);
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

void ExecutionEngine::changeProcessImage(Command &command, std::string fullPath) {
	chdir(environment.getCurrentDir().c_str());

	for (auto &redirection : command.redirections) {
		handleRedirection(redirection);
	}

	char **args = command.argsToArr();

	int ret = execv(fullPath.c_str(),
					args);

	// If exec fails, we need to exit the process
	// TODO: different exeptions
	throw std::logic_error("Error executing new process: " + command.command + ": " + strerror(errno));
}

void ExecutionEngine::handleRedirection(Command::Redirection &redirection) {
	int flags = O_CREAT;

	if (redirection.input) {
		flags |= O_RDONLY;

		if (redirection.index == 1 || redirection.index == 0) {
			throw std::logic_error("Can't redirect decriptor 1 or 2 as input " +
			                         std::to_string(redirection.index) + ": " + strerror(errno));
		}

	} else {
		flags |= O_WRONLY;

		if (redirection.index == 0) {
			throw std::logic_error("Can't redirect decriptor 0 as output " +
			                         std::to_string(redirection.index) + ": " + strerror(errno));
		}

		if (redirection.endOfFile)
			flags |= O_APPEND;
	}


	int fd = open(redirection.fileName.c_str(), flags, S_IRUSR | S_IWUSR);

	if (fd < 0) {
		throw std::logic_error("Problem opening file: " + redirection.fileName + ": " + strerror(errno));
	}

	int ret = dup2(fd, redirection.index);
	if (ret < 0) {
		throw std::logic_error("Problem redirecting file descriptor nr: " +
		                         std::to_string(redirection.index) + ": " + strerror(errno));
	}
}


