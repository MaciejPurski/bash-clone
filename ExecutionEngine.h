#ifndef BASH_CLONE_ENGINE_H
#define BASH_CLONE_ENGINE_H

#include <stack>
#include "Environment.h"
#include "Command.h"
#include "Job.h"
#include <list>

/**
 * @class ExecutionEngine
 * @brief Bash execution engine class
 *
 * The class is responsible for preparing commands to be ran by
 * creating an abstraction of a pipeline - *job*.
 *
 * It takes care of executing built-in commands and managing
 * job control system.
 */
class ExecutionEngine {
private:
	enum Direction {IN, OUT};
	Environment &environment;
	std::deque<Job> jobs;
	int nextJobNumber;

	void addJob(std::vector<Command> &pipe);

	/* Built-in commands */
	void executeBuiltIn(Command &command);
	void cdCommand(Command &command);
	void envCommand(Command &command);
	void exportCommand(Command &command);
	void jobsCommand();

	std::string checkCommand(Command &command);

public:
	/**
	 * @brief Basic Execution engine constructor
	 * @param e Reference to the Environment class
	 */
    ExecutionEngine(Environment &e);

    /**
     * @brief Basic method of the Engine class used for commands execution
     *
     * The method checks commands correctness and calls the environment in
     * order to resolve commands name and paths. It recognizes if the command is
     * a built-in. It then looks for pipes in the command line and creates a new
     * job, which is put on the jobs queue and executed in the background or in the foreground.
     * @param commands A vector of commands to be executed
     */
    void executeCommandLine(std::vector<Command> &commands);

    /**
     * @brief Update jobs' state
     *
     * The method iterates over all jobs and calls for updating their
     * state.
     */
	void update();

	/**
	 * @brief Show all background jobs which has been done
	 *
	 * Called in the prompt method in order to show done backround jobs,
	 * in the Shell's prompt method, when the user presses ENTER.
	 */
	void showDoneBackgroundJobs();

	/**
	 * @brief Remove done jobs
	 */
	void jobsCleanup();

	int getNJobs();
};


#endif //BASH_CLONE_ENGINE_H
