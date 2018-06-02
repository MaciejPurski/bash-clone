#ifndef BASH_CLONE_JOB_H
#define BASH_CLONE_JOB_H

#include <string>
#include <vector>
#include <unistd.h>
#include "Command.h"

enum State {Stopped, Running, Done};

class Job {
private:
	enum State state;
	pid_t gpid;
	std::vector<Command> commands;
	std::vector<pid_t> pids;
	std::string commandLine;
	bool foreground;
public:

	Job(std::vector<Command> &commands) : commands(commands) {

	}

	void setStopped();

	void runForegroud();

	void setDone();

	void runBackground();

	void handleRedirection(Command::Redirection &redirection);

	void changeProcessImage(Command &command, bool foreground);

	void start(std::string currentDir);
};


#endif //BASH_CLONE_JOB_H
