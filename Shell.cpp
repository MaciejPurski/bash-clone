#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <termio.h>
#include "Shell.h"

void Shell::start() {
	if (interactive) {
		/* Loop until, we are in the foreground */
		while (tcgetpgrp(STDIN_FILENO) != (shellProcessGroup = getpgrp()))
			kill(-shellProcessGroup, SIGTTIN);

		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);

		/* Our shell should become a new group's leader */
		if (setpgid(shellProcessGroup, shellProcessGroup) < 0) {
			throw std::runtime_error("Can't change process group");
		}

		/* Set our new group to be the foreground group of the terminal */
		tcsetpgrp(STDIN_FILENO, shellProcessGroup);

		run(initFile, false);
		run(std::cin, true);
	} else {
		run(scriptFile, false);
	}
}

void Shell::run(std::istream &input, bool interactiveMode) {
	std::string line;

	/* buildLine() returns an empty string in case of an end of buffer */
	while (!(line = buildLine(input, interactiveMode)).empty()) {
		try {
			std::vector<Command> commands = interpreter.processCommandLine(line);
			engine.executeCommandLine(std::move(commands));
			tcsetpgrp(STDIN_FILENO, shellProcessGroup);
		} catch (std::logic_error &e) {
			std::cerr << "Process error: " << e.what() << std::endl;
			exit(1);
		} catch (std::exception &e) {
			// TODO: exceptions
			std::cerr << e.what() << std::endl;
		}
	}

	if (interactiveMode)
		std::cout << "\nExiting\n";
}


void Shell::prompt() {
	engine.update();
	engine.showDoneBackgroundJobs();
	engine.jobsCleanup();
	std::cout << strToGreen(env.getValue("USER")) << ":" << strToBlue(env.getCurrentDir()) << "$ ";
}

std::string Shell::buildLine(std::istream &input, bool interactiveMode) {
	std::string line;

	/* wait for user input */
	do {
		if (interactiveMode)
			prompt();

		/* End of buffer reached */
		if (!std::getline(input, line))
			return "";

	} while (line.empty());

	/* Create a multiline command line */
	while (line.back() == '\\' || line.back() == '|'
	       || ((std::count(line.begin(), line.end(), '\'') % 2) != 0)) {
		std::string continuation;

		if (line.back() == '\\')
			line.pop_back();

		if (interactiveMode)
			std::cout << ">  ";

		if (!std::getline(input, continuation))
			return "";
		line.append(continuation);
	}

	return line;
}