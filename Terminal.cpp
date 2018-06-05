#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <termio.h>
#include "Terminal.h"

#define SIGSTP 20

static Terminal *t;

void Terminal::start() {
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

void Terminal::run(std::istream &input, bool interactiveMode) {
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


void Terminal::prompt() {
	engine.update();
	engine.showDoneBackgroundJobs();
	engine.jobsCleanup();
	std::cout << strToGreen(env.getValue("USER")) << ":" << strToBlue(env.getCurrentDir()) << "$ ";
}


void signalHandler(int signal) {
	switch (signal) {

		case SIGINT:
			t->sigint();
			break;
		case SIGSTP:
			t->sigstp();
			break;
		case SIGQUIT:
			t->sigquit();
			break;
		default:
			std::cerr << "UNKOWN SIGNAL\n";
	}
}


void Terminal::installSignals() {
	struct sigaction sa;

	t = this;

	sa.sa_handler = &signalHandler;

	// Restart the system call, if at all possible
	sa.sa_flags = SA_RESTART;

	// Block every signal during the handler
	sigfillset(&sa.sa_mask);


	if (sigaction(SIGINT, &sa, nullptr) == -1) {
		perror("Error: cannot handle SIGINT"); // Should not happen
	}

	if (sigaction(SIGQUIT, &sa, nullptr) == -1) {
		perror("Error: cannot handle SIGQUIT"); // Should not happen
	}

	if (sigaction(SIGSTP, &sa, nullptr) == -1) {
		perror("Error: cannot handle SIGSTP"); // Should not happen
	}
}

void Terminal::sigquit() {
	engine.sendSignal(SIGQUIT);

	std::cout << "\nQuit(core dumped)\n";
	t->prompt();
	std::cout.flush();
}

void Terminal::sigint() {
	engine.sendSignal(SIGINT);
	std::cout << std::endl;
	t->prompt();
	std::cout.flush();
}

void Terminal::sigstp() {
	engine.sendSignal(SIGSTP);
	std::cout << "\nStopped\n";
	t->prompt();
	std::cout.flush();
}
