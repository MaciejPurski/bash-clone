#ifndef BASH_CLONE_TERMINAL_H
#define BASH_CLONE_TERMINAL_H


#include <fstream>
#include "Interpreter.h"
#include "Environment.h"
#include "ExecutionEngine.h"

class Terminal {
private:
	Environment env;
	Interpreter interpreter;
	ExecutionEngine engine;
	std::ifstream initFile, scriptFile;
	void run(std::istream &input, bool interactiveMode);
	bool interactive;

public:
	Terminal(const std::string &initFilename, const std::string &scriptFilename) : interpreter(env), engine(env) {
		initFile.open(initFilename);

		if (scriptFilename != "") {
			scriptFile.open(scriptFilename);
			interactive = false;
		} else {
			interactive = true;
		}
	}

	void start();

};


#endif
