#include "Job.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

void Job::start(std::string currentDir) {
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

		// TODO: different group leaders
		setpgid(pid, pid);
		tcsetpgrp(STDIN_FILENO, pid);


		fflush(stdout);

		// TODO: dont wait if exec failed

		int status;
		// TODO: waiting for any process in the group, not just last
		if (&cmd == &commands.back() && !cmd.isBackgroud()) {
			waitpid(pid, &status, WUNTRACED);
			/* close pipes and unlink them */
			for (int i = 0; i < pipesFd.size(); i++) {
				close(pipesFd[i]);
				unlink(cmd.pipes[i].fileName.c_str());
			}

		}
	}
}


void Job::changeProcessImage(Command &command, bool foreground) {

	/* Put the process into the process group and give the process group
         the terminal, if appropriate.
         This has to be done both by the shell and in the individual
         child processes because of potential race conditions.  */
	pid_t pid = getpid();

	// TODO: process group leaders
	//if (pgid == 0) pgid = pid;
	/* set process in its own group */
	setpgid(pid, pid);
	if (foreground)
		tcsetpgrp(STDIN_FILENO, pid);


	for (auto &redirection : command.redirections) {
		handleRedirection(redirection);
	}

	char **args = command.argsToArr();

	/* Set the handling for job control signals back to the default.  */
	signal (SIGINT, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	signal (SIGTTIN, SIG_DFL);
	signal (SIGTTOU, SIG_DFL);
	signal (SIGCHLD, SIG_DFL);

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