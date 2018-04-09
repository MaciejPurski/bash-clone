#ifndef BASH_CLONE_COMMAND_H
#define BASH_CLONE_COMMAND_H

#include <iostream>
#include <vector>

/**
 * Abstract class representing a command. Only its
 * derived classes can be instantiated.
 */
class Command {
public:
	virtual ~Command() {};

	/**
	 * used to determine class type without
	 * using ineffective RTTI mechanisms.
	 */
	enum CommandType {EXECUTABLE, ASSIGN};

	CommandType getType() {
		return type;
	};

protected:
	Command() {};
	CommandType type;
};

class AssignCommand : public Command {
public:
	std::string name;
	std::string value;

	AssignCommand() {
		Command::type = ASSIGN;
		name = "";
		value = "";
	}

	AssignCommand(const std::string &nName, const std::string &nValue) :
			name(nName), value(nValue) {
		Command::type = ASSIGN;
	}
};



class ExecutableCommand : public Command {
public:
	struct Redirection {
		int index;
		bool input;
		std::string fileName;
	};

	enum CommandTermination {TERM, PIPE, AMPER};

	std::string command;
	std::vector<std::string> args;
	std::vector<Redirection> redirections;
	CommandTermination term;

	ExecutableCommand() {
		Command::type = EXECUTABLE;
		command = "";
		term = TERM;
	}
};

#endif
