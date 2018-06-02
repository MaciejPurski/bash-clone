#include "Job.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>

void Job::start(std::string currentDir) {
	pipeProcess();
	foreground = !commands.back().isBackgroud();

	for (auto &cmd : commands) {
		std::vector<int> pipesFd;

		pid_t pid;
		if ((pid = fork()) == 0) {
			for (auto &f : cmd.pipes) {
				int flags = 0;
				if (f.input)
					flags = O_RDONLY;
				else
					flags = O_WRONLY;
				pipesFd.push_back(open(f.fileName.c_str(), flags));
			}

			chdir(currentDir.c_str());

			for (int i = 0; i < pipesFd.size(); i++) {
				dup2(pipesFd[i], cmd.pipes[i].index);
			}

			changeProcessImage(cmd, !commands.back().isBackgroud());
		} else if (pid < 0) {
			throw std::runtime_error("Problem while forking new process: " + std::string(strerror(errno)));
		}

		if (pgid == 0)
			pgid = pid;
		setpgid(pid, pgid);
	}

	if (foreground)
		runForeground(false);
	else
		runBackground(false);
}


void Job::changeProcessImage(Command &command, bool foreground) {

	/* Put the process into the process group and give the process group
         the terminal, if appropriate.
         This has to be done both by the shell and in the individual
         child processes because of potential race conditions.  */
	pid_t pid = getpid();

	if (pgid == 0)
		pgid = pid;

	setpgid(pid, pgid);

	if (foreground)
		tcsetpgrp(STDIN_FILENO, pid);


	for (auto &redirection : command.redirections) {
		handleRedirection(redirection);
	}

	char **args = command.argsToArr();

	/* Set the handling for job control signals back to the default.  */
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);

	// TODO: environment
	int ret = execv(command.fullPath.c_str(),
	                args);

	// If exec fails, we need to exit the process
	// TODO: different exeptions
	throw std::logic_error("Error executing new process: " + command.command + ": " + strerror(errno));
}

void Job::handleRedirection(Command::Redirection &redirection) {
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

/*
 * Method changes commands redirections in order to instantiate a pipe
 */
void Job::pipeProcess() {
	for (int i = 0; i < commands.size() - 1; i++) {
		std::string pipeName = pipeOpen(commands[i].command);

		pipes.push_back(pipeName);

		/* output pipe */
		commands[i].pipes.push_back(Command::Redirection(1, false, false, pipeName));

		/* input pipe */
		commands[i + 1].pipes.push_back(Command::Redirection(0, true, false, pipeName));
	}
}

std::string Job::pipeOpen(std::string src) {
	int fd = -1;
	std::string fifoName;

	srand(time(NULL));

	// in case the file exists, try to generate a new name for it
	do {
		int randomNumber = rand();

		if (fd > 0)
			close(fd);

		fifoName = "/tmp/" + src + std::to_string(randomNumber);
	} while (open(fifoName.c_str(), 0) > 0);

	fd = mknod(fifoName.c_str(), S_IFIFO|0666, 0);

	if (fd < 0)
		throw std::runtime_error("Can't create fifo: " + std::string(strerror(errno)));

	return fifoName;
}

void Job::runForeground(bool cont) {
	if (!foreground || cont)
		std::cout << commandLine << std::endl;

	/* Put the job into the foreground.  */
	tcsetpgrp(STDIN_FILENO, pgid);
	running = commands.size();
	state = Running;
	foreground = true;

	fflush(stdout);
	/* Send the job a continue signal, if necessary.  */
	if (cont) {
		if (kill(-pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");

	}

	int status;
	pid_t pid;
	/* Wait for it to report.  */
	while (state == Running) {
		pid = waitpid(-pgid, &status, WUNTRACED);

		if (WIFSTOPPED(status)) {
			running = 0;
			state = Stopped;
			foreground = false;
			std::cout << std::endl;
			showJob();
		} else if (WIFSIGNALED(status)) {
			state = Done;
		} else {
			running--;
			if (running == 0)
				state = Done;
		}
	}
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

pid_t Job::getPid() {
	return pgid;
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
		int status;
		pid_t pid = waitpid(-pgid, &status, WUNTRACED | WNOHANG);

		/* nothing has happened */
		if (pid == 0)
			return;

		// TODO: KILLED
		if (WIFSTOPPED(status)) {
			running = 0;
			state = Stopped;
			foreground = false;
		} else if (WIFSIGNALED(status)) {
			state = Done;
		} else {
			running--;
			// TODO: set returning code
			if (running == 0)
				state = Done;
		}
	}
}