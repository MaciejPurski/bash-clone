#include "Job.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>

void Job::start(const Environment &env) {
	pipeProcess();
	foreground = !commands.back().isBackgroud();

	for (auto &cmd : commands) {
		std::vector<int> pipesFd;

		pid_t pid;
		if ((pid = fork()) == 0) {
			changeProcessImage(cmd, !commands.back().isBackgroud(), env);
		} else if (pid < 0) {
			throw EngineException("Problem while forking new process: " + std::string(strerror(errno)));
		}

		if (pgid == 0)
			pgid = pid;
		setpgid(pid, pgid);
	}

	running = commands.size();
	if (foreground)
		runForeground(false);
	else
		runBackground(false);
}


/**
 * Put the process into the process group and give the process group
 *	 the terminal, if appropriate.
 *	 This has to be done both by the shell and in the individual
 *	 child processes because of potential race conditions.
 */
void Job::changeProcessImage(Command &command, bool foreground, const Environment &env) {
	pid_t pid = getpid();

	chdir(env.getCurrentDir().c_str());

	if (pgid == 0)
		pgid = pid;

	setpgid(pid, pgid);

	if (foreground)
		tcsetpgrp(STDIN_FILENO, pgid);

	for (auto &redirection : command.redirections) {
		handleRedirection(redirection);
	}

	for (auto &redirection : command.pipes) {
		handleRedirection(redirection);
	}

	setDefaultSignalsHandling();

	int ret = execve(command.fullPath.c_str(),
	                 command.argsToArr(),
					 env.getEnvironment());

	// If exec fails, we need to exit the process
	// TODO: different exeptions
	throw ChildProcessException("Error executing new process: " + command.command + ": " + strerror(errno));
}

void Job::handleRedirection(Command::Redirection &redirection) {
	int flags = O_CREAT;

	if (redirection.input) {
		flags |= O_RDONLY;

		if (redirection.index == 1 || redirection.index == 2) {
			throw ChildProcessException("Can't redirect decriptor 1 or 2 as input " +
			                       std::to_string(redirection.index) + ": " + strerror(errno));
		}

	} else {
		flags |= O_WRONLY;

		if (redirection.index == 0) {
			throw ChildProcessException("Can't redirect decriptor 0 as output " +
			                       std::to_string(redirection.index) + ": " + strerror(errno));
		}

		if (redirection.endOfFile)
			flags |= O_APPEND;
	}


	int fd = open(redirection.fileName.c_str(), flags, S_IRUSR | S_IWUSR);

	if (fd < 0) {
		throw ChildProcessException("Problem opening file: " + redirection.fileName + ": " + strerror(errno));
	}

	int ret = dup2(fd, redirection.index);
	if (ret < 0) {
		throw ChildProcessException("Problem redirecting file descriptor nr: " +
		                       std::to_string(redirection.index) + ": " + strerror(errno));
	}
}

/*
 * Method changes commands redirections in order to instantiate a pipe
 */
void Job::pipeProcess() {
	for (int i = 0; i < commands.size() - 1; i++) {
		std::string pipeName = makePipe(commands[i].command);

		pipes.push_back(pipeName);

		/* output pipe */
		commands[i].pipes.emplace_back(Command::Redirection(1, false, false, pipeName));

		/* input pipe */
		commands[i + 1].pipes.emplace_back(Command::Redirection(0, true, false, pipeName));
	}
}

std::string Job::makePipe(std::string src) {
	int fd = -1;
	std::string fifoName;

	srand(time(NULL));

	/* in case the file exists, try to generate a new name for it */
	do {
		int randomNumber = rand();

		if (fd > 0)
			close(fd);

		fifoName = "/tmp/" + std::to_string(randomNumber);
	} while (access(fifoName.c_str(), F_OK) == 0);

	fd = mknod(fifoName.c_str(), S_IFIFO|0666, 0);

	if (fd < 0)
		throw EngineException("Can't create fifo: " + std::string(strerror(errno)));

	return fifoName;
}

void Job::runForeground(bool cont) {
	if (!foreground || cont)
		std::cout << commandLine << std::endl;

	/* Put the job into the foreground.  */
	tcsetpgrp(STDIN_FILENO, pgid);
	state = Running;
	foreground = true;

	fflush(stdout);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		if (kill(-pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");
	}

	/* Wait for it to report.  */
	while (state == Running)
		jobWait(WUNTRACED);
}

void Job::runBackground(bool cont) {
	foreground = false;
	state = Running;

	std::cout << "[" << number << "] ";

	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		std::cout << commandLine + " & ";
		if (kill(-pgid, SIGCONT) < 0)
			perror("kill (SIGCONT)");
	} else {
		std::cout << pgid;
	}

	std::cout << std::endl;
}

bool Job::isDone() {
	return state == Done;
}

bool Job::isStopped() {
	return state == Stopped;
}

std::string Job::getState() {
	switch (state) {
		case Running:
			return "Running";
		case Done:
			return "Done";
		case Stopped:
			return "Stopped";
	}
}

void Job::showJob() {
	std::cout << "[" << number << "]" << " " << pgid << " " << getState() << " " << commandLine;
	if (!foreground)
		std::cout << " &";
	std::cout << std::endl;
}

void Job::updateState() {
	if (state == Done)
		return;

	if (!foreground) {
		jobWait(WUNTRACED | WNOHANG);
	}
}

void Job::setDefaultSignalsHandling() {
	/* Set the handling for job control signals back to the default.  */
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);
}

int Job::getStatus() {
	return retStatus;
}

void Job::jobWait(int flags) {
	int status;
	pid_t pid;

	pid = waitpid(-pgid, &status, flags);
	/* Nothing happened */
	if (pid == 0)
		return;

	if (pid < 0 && !(flags & WNOHANG))
		perror("wait problem");

	if (WIFSTOPPED(status)) {
		state = Stopped;
		std::cout << std::endl;
		showJob();
	} else {
		running--;

		if (!(flags & WNOHANG))
			retStatus = WEXITSTATUS(status);

		if (running == 0)
			state = Done;
	}
}
