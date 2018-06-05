#ifndef BASH_CLONE_TERMINAL_H
#define BASH_CLONE_TERMINAL_H


#include <fstream>
#include <termios.h>
#include "Interpreter.h"
#include "Environment.h"
#include "ExecutionEngine.h"

class Shell {
private:
	Environment env;
	Interpreter interpreter;
	ExecutionEngine engine;
	std::ifstream initFile, scriptFile;
	pid_t shellProcessGroup;
	struct termios shellModes;
	void run(std::istream &input, bool interactiveMode);
	bool interactive;

	static inline std::string strToBlue(const std::string &str) {
		return  std::string("\033[1;34m" + str + "\033[0m");
	}

	static inline std::string strToGreen(const std::string &str) {
		return  std::string("\033[1;32m" + str + "\033[0m");
	}

public:
	Shell(const std::string &initFilename, const std::string &scriptFilename) : engine(env), interpreter(env) {
		initFile.open(initFilename);

		if (!scriptFilename.empty()) {
			scriptFile.open(scriptFilename);
			interactive = false;
		} else {
			interactive = true;
		}
	}

	void start();
	void prompt();
};


#endif