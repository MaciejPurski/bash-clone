#ifndef BASH_CLONE_COMMAND_H
#define BASH_CLONE_COMMAND_H

#include <iostream>
#include <vector>
#include <cstring>

/**
 * @brief Class representing a command.
 */
class Command {
public:
	struct Redirection {
		Redirection(int index, bool input, bool endOfFile, const std::string &fileName) : index(index), input(input),
																						  endOfFile(endOfFile),
																						  fileName(fileName) {}

		bool operator==(const Redirection &rhs) const {
			return index == rhs.index &&
				   input == rhs.input &&
				   endOfFile == rhs.endOfFile &&
				   fileName == rhs.fileName;
		}

		int index;
		bool input;
		bool endOfFile;
		std::string fileName;
	};

	enum CommandTermination {TERM, PIPE, AMPER};

	std::string command;
	std::vector<std::string> args;
	std::vector<Redirection> redirections;
	std::vector<Redirection> pipes;
	std::string fullPath;
	CommandTermination term;

	Command() {
		command = "";
		term = TERM;
	}

	char **argsToArr() {
		char ** arr = new char *[args.size() + 2];

		arr[0] = new char [command.size() + 1];
		strcpy(arr[0], command.c_str());

		int i = 0;
		for (; i < args.size(); i++) {
			arr[i + 1] = new char [args[i].size() + 1];
			strcpy(arr[i + 1], args[i].c_str());
		}

		arr[i + 1] = NULL;
		return arr;
	}

	bool pipeTerminated() {
		return term == PIPE;
	}

	bool isBackgroud() {
		return term == AMPER;
	}

	void setFullPath(std::string str) {
		fullPath = str;
	}

	bool isBuiltIn() {
			return command == "cd" || command == "pwd" ||
			       command == "env" ||
			       command == "bg" || command == "fg" ||
			       command == "jobs" || command == "export";

	}

	bool operator==(const Command &rhs) const {
		return command == rhs.command &&
			   args == rhs.args &&
			   redirections == rhs.redirections &&
			   term == rhs.term;
	}

};



#endif
