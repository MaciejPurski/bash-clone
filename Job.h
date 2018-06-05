#ifndef BASH_CLONE_JOB_H
#define BASH_CLONE_JOB_H

#include <string>
#include <vector>
#include <unistd.h>
#include "Command.h"
#include "Environment.h"

enum State {Stopped, Running, Done};

/**
 * @class Jobs
 *
 * @brief An abstraction for a pipeline
 *
 * Class represents a process pipeline. It keeps the current state
 * of the pipe, is responsible for putting the pipe in the foreground or
 * in the background, for updating its state and printing information.
 */
class Job {
private:
	std::vector<std::string> pipes;
	enum State state;
	pid_t pgid;
	std::vector<Command> commands;
	std::string commandLine;
	int running = 0;
	bool foreground;
	int number;
	int retStatus;

	std::string pipeOpen(std::string src);
	void pipeProcess();
	void changeProcessImage(Command &command, bool foreground, const Environment &env);
	void handleRedirection(Command::Redirection &redirection);
	void jobWait(int flags);
	void setDefaultSignalsHandling();
public:

	/**
	 * @brief Basic job constructor
	 * @param ncommands Vector of commands
	 * @param number Number of a job set by the ExecutionEngine
	 */
	Job(std::vector<Command> &ncommands, int number) : commands(std::move(ncommands)), number(number) {
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

   /**
    *
    * @return True if the job has been ran in the background
    */
	bool isBackground() {
		return !foreground;
	}

	/**
	 * @brief Get job's number
	 * @return Job's number as set by the ExecutionEngine
	 */
	int getNumber() {
		return number;
	}

	/**
	 * @brief Get command line, which was used to create the job
	 * @return A printable command line
	 */
	std::string getCommandLine() {
		return commandLine;
	}

	/**
	 * @brief Start the job
	 * @param env Environment used to obtain environmental variables
	 */
	void start(const Environment &env);

   /**
	* @brief Runs the job in the foreground
	*
	* Bash will be waiting for the job and the job will be receiving
	* signals from the terminal.
	* @param cont Should the SIGCONT be sent to the process. Set to True,
	* 		       if the process has been stopped and is about to be continued.
	*/
	void runForeground(bool cont);

   /**
 	* @brief Runs the job in the background
 	*
 	* Bash won't be waiting for the job to get finished and the user will
    * still be able to exec new commands. The background job won't be receiving
    * signals and if it tries to access stdin, it will be stopped.
    *
 	* @param cont Should the SIGCONT be sent to the process. Set to True,
 	* 		       if the process has been stopped and is about to be continued.
 	*/
	void runBackground(bool cont);

	/**
	 * @brief Check if the job is done
	 * @return True if the whole job has been finished, False otherwise
	 */
	bool isDone();

	/**
	 * @brief Check if the job has been stopped
	 * @return True if the job has been stopped by the SIGSTP signal
	 */
	bool isStopped();

	/**
	 * @brief Get current process state
	 * @return A printable string representing job's state
	 */
	std::string getState();

	/**
	 * @brief Print job number, current state, command line and pid
	 */
	void showJob();

	/**
	 * @brief Update background job's state
	 *
	 * The function performs waitpid() with WNOHANG flag, so it is able
	 * to get process state change without blocking. It should be called,
	 * whenever the user needs to update job's state.
	 */
	void updateState();

	/**
	 * Get last command return code. It should be used only for foreground jobs.
	 * @return Last command's return status or 0
	 */
	int getStatus();
};


#endif //BASH_CLONE_JOB_H
