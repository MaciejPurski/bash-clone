#include "Engine.h"

#include <iostream>

int main(int argc, char *argv[])
{
	Interpreter i;
	Environment env;
	Engine e(i, env, false);
	e.run();
	std::cout<<"path "<<env.searchPath("sleep")<<std::endl;

	return 0;
}