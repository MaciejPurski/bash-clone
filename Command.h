#ifndef BASH_CLONE_COMMAND_H
#define BASH_CLONE_COMMAND_H

#include <iostream>
#include <vector>
#include <cstring>

/**
 * Class representing a command.
 */
class Command {
public:
	struct Redirection {
		Redirection(int index, bool input, bool endOfFile, const std::string &fileName) : index(index), input(input),
																						  endOfFile(endOfFile),
																						  fileName(fileName) {}

		int index;
		bool input;
		bool endOfFile;
		std::string fileName;
	};

	enum CommandTermination {TERM, PIPE, AMPER};

	std::string command;
	std::vector<std::string> args;
	std::vector<Redirection> redirections;
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

};



#endif
