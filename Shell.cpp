#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <termio.h>
#include "Shell.h"

#define SIGSTP 20

static Shell *t;

void Shell::start() {
	// init script

	if (interactive) {
		/* Loop until, we are in the foreground */
		while (tcgetpgrp(STDIN_FILENO) != (shellProcessGroup = getpgrp()))
			kill (-shellProcessGroup, SIGTTIN);

		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTTOU, SIG_IGN);
		signal (SIGCHLD, SIG_IGN);

		shellProcessGroup = getpid();
		/* Our shell should become a new group's leader */
		if (setpgid(shellProcessGroup, shellProcessGroup) < 0)
		{
			perror ("Couldn't put the shell in its own process group");
			exit (1);
		}

		/* Set our new group to be the foreground group of the terminal */
		tcsetpgrp(STDIN_FILENO, shellProcessGroup);

		/* Save default terminal attributes for shell.  */
		tcgetattr(shellProcessGroup, &shellModes);
		run(initFile, false);
		run(std::cin, true);
	} else {
		run(scriptFile, false);
	}
}

void Shell::run(std::istream &input, bool interactiveMode) {
	std::string line;

	while (true) {
		if (interactiveMode)
			prompt();

		if (!std::getline(input, line)) {
			std::cout << std::endl;
			break;
		}

		// if a continuation is needed
		while (line.back() == '\\' || line.back() == '|'
		       || ((std::count(line.begin(), line.end(), '\'') % 2) != 0)) {
			std::string continuation;

			if (line.back() == '\\')
				line.pop_back();

			if (interactiveMode)
				std::cout << ">  ";
			std::getline(input, continuation);
			line.append(continuation);
		}

		if (line.empty())
			continue;

		try {
			std::vector<Command> commands = interpreter.processCommandLine(line);
			engine.executeCommandLine(std::move(commands));
			tcsetpgrp(STDIN_FILENO, shellProcessGroup);
			//tcgetattr (shell_terminal, &j->tmodes);
			//tcsetattr (STDIN_FILENO, TCSADRAIN, &shellModes);

		} catch (std::logic_error &e) {
			std::cerr << "Process error: " << e.what() << std::endl;
			exit(1);
		} catch (std::exception &e) {
			// TODO exception handling
			std::cerr << e.what() << std::endl;
		}
	}
}


void Shell::prompt() {
	engine.update();
	engine.showDoneBackgroundJobs();
	engine.jobsCleanup();
	std::cout << strToGreen(env.getValue("USER")) << ":" << strToBlue(env.getCurrentDir()) << "$ ";
}
