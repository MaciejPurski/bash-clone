#ifndef BASH_CLONE_TERMINAL_H
#define BASH_CLONE_TERMINAL_H


#include <fstream>
#include <termios.h>
#include "Interpreter.h"
#include "Environment.h"
#include "ExecutionEngine.h"

/**
 *	@class Shell
 *
 *	@brief Bash main class responsible for basic program flow
 *	It reads the user input, creates lines and send them to Interpreter
 *	and then Engine to execute. It allows running in both interactive
 *	and non-interactive mode.
 */
class Shell {
private:
	Environment env;
	Interpreter interpreter;
	ExecutionEngine engine;
	std::ifstream initFile, scriptFile;
	pid_t shellProcessGroup;
	bool interactive;

	void run(std::istream &input, bool interactiveMode);

	static inline std::string strToBlue(const std::string &str) {
		return  std::string("\033[1;34m" + str + "\033[0m");
	}

	static inline std::string strToGreen(const std::string &str) {
		return  std::string("\033[1;32m" + str + "\033[0m");
	}

	void prompt();
	std::string buildLine(std::istream &input, bool interactive);

public:
	/**
	 * @brief Shell constructor. It shall be called only once
	 * @param initFilename A path to init script, which is ran at the beggining
	 * 					   of the program execution. It is required to provide it.
	 * 					   It is recommended that the init script contains a definition of PATH variable
	 *
	 * @param scriptFilename An optional argument, when provided, makes the shell running
	 * 						 in a non-interactive script mode
	 */
	Shell(const std::string &initFilename, const std::string &scriptFilename = "") : engine(env), interpreter(env) {
		shellProcessGroup = getpid();

		initFile.open(initFilename);
		if (!initFile)
			throw std::runtime_error("Can't open file: " + initFilename);

		if (!scriptFilename.empty()) {
			scriptFile.open(scriptFilename);
			if (!scriptFile)
				throw std::runtime_error("Can't open file: " + initFilename);
			interactive = false;
		} else {
			interactive = true;
		}
	}

	/**
	 * @brief Starts the bash.
	 *
	 * This function is responsible for initializing the bash process.
	 * If it is ran in interactive mode, it waits until the main bash process
	 * is in the foreground in order to avoid being stopped by SIGTTIN. It then
	 * sets the signals to be ignored, it puts the bash process in its own process group,
	 * runs the init script and the main program flow (interactive or non-interactive).
	 */
	void start();
};


#endif
