#ifndef BASH_CLONE_COMMAND_H
#define BASH_CLONE_COMMAND_H

#include <iostream>
#include <vector>

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

};



#endif
