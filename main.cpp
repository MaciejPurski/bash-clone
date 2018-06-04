#include <cstring>
#include "ExecutionEngine.h"
#include "Terminal.h"

#include <iostream>

int main(int argc, char *argv[])
{
	std::string initName;
	std::string scriptName;

	try {
		Environment env;
		std::cout << env.expandPath("~/Obrazy/Makefile") << std::endl;
		std::cout << env.getValue("PATH") << std::endl;
		std::cout << env.getValue("OTHER") <<std::endl;
	}
	catch(EnvironmentException &ex)
	{
		std::cout<<"EX\n";
		std::cout<<ex.what()<<std::endl;
	}
	/*
	if (argc < 2) {
		std::cout << "Not enough arguments\nUsage: bash-clone <init_script_name> [optional] -c <script_name>\n";
		exit(1);
	}

	initName = argv[1];

	if (argc == 4) {
		if (strcmp(argv[2], "-c") != 0) {
			std::cout << "Wrong argument. Only -c option accepted\n";
			exit(1);
		}

		scriptName = argv[3];

	} else if (argc != 2) {
		std::cout << "Wrong arguments Usage: bash-clone <init_script_name> [optional] -c <script_name>\n";
	}

	Terminal t(initName, scriptName);
	t.start();
*/
	return 0;
}