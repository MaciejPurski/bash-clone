#ifndef BASH_CLONE_JOB_H
#define BASH_CLONE_JOB_H

#include <string>
#include <vector>
#include <unistd.h>
#include "Command.h"
#include "Environment.h"

enum State {Stopped, Running, Done};

class Job {
private:
	enum State state;
	pid_t pgid;
	std::vector<Command> commands;
	std::string commandLine;
	int running = 0;
	std::vector<std::string> pipes;
	bool foreground;
	int number;

	std::string pipeOpen(std::string src);
	void pipeProcess();
	void changeProcessImage(Command &command, bool foreground);
public:

	Job(std::vector<Command> &commands, int number) : commands(std::move(commands)), number(number) {
		pgid = 0;
		for (auto &c : commands) {
			commandLine += c.command + " ";

			for (auto &arg : c.args) {
				commandLine += arg + " ";
			}

			if (c.pipeTerminated())
				commandLine += "| ";
		}
	}

	~Job() {
		for (auto &p : pipes)
			unlink(p.c_str());
	}

	void setStopped();

	void runForeground();

	void setDone();

	void runBackground();

	int getNumber() {
		return number;
	}

	std::string getCommandLine() {
		return commandLine;
	}



	void handleRedirection(Command::Redirection &redirection);
	void start(const Environment &env);

	void runForeground(bool cont);

	bool isDone();

	void runBackground(bool cont);

	bool isStopped();

	std::string getState();

	pid_t getPid();

	void showJob();

	void updateState();

	void showDoneBackgroundJobs();

	bool isBackground() {
		return !foreground;
	}

	void changeProcessImage(Command &command, bool foreground, char **environment);

	void changeProcessImage(Command &command, bool foreground, const Environment &env);

	void setDefaultSignalsHandling();
};


#endif //BASH_CLONE_JOB_H
