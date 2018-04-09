#include "Engine.h"

int main(int argc, char *argv[])
{
	Interpreter i;
	Environment env;
	Engine e(i, env, false);

	e.run();

	return 0;
}